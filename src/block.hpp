//--------------------------------------------------------------
// one diy block
//
// Tom Peterka
// Argonne National Laboratory
// tpeterka@mcs.anl.gov
//--------------------------------------------------------------

#include    <mfa/mfa.hpp>
#include    <mfa/block_base.hpp>

#include    <diy/master.hpp>
#include    <diy/reduce-operations.hpp>
#include    <diy/decomposition.hpp>
#include    <diy/assigner.hpp>
#include    <diy/io/block.hpp>
#include    <diy/io/bov.hpp>
#include    <diy/pick.hpp>

#include    <stdio.h>

#include    <Eigen/Dense>

#include    <random>

using namespace std;

// set input and ouptut precision here, float or double
#if 0
typedef float                          real_t;
#else
typedef double                         real_t;
#endif

// 3d point or vector
struct vec3d
{
    float x, y, z;
    float mag() { return sqrt(x*x + y*y + z*z); }
    vec3d(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    vec3d() {}
};

// arguments to block foreach functions
struct DomainArgs : public ModelInfo
{
    DomainArgs(int dom_dim, int pt_dim) :
        ModelInfo(dom_dim, pt_dim)
    {
        tot_ndom_pts = 0;
        starts.resize(dom_dim);
        ndom_pts.resize(dom_dim);
        full_dom_pts.resize(dom_dim);
        min.resize(dom_dim);
        max.resize(dom_dim);
        s.resize(pt_dim, 1.0);  // construct with all 1s
        f.resize(pt_dim, 1.0);  // construct with all 1s
        r = 0;
        t = 0;
        n = 0;
        multiblock = false;
        structured = true;   // Assume structured input by default
        rand_seed  = -1;
    }
    size_t              tot_ndom_pts;
    vector<int>         starts;                     // starting offsets of ndom_pts (optional, usually assumed 0)
    vector<int>         ndom_pts;                   // number of points in domain (possibly a subset of full domain)
    vector<int>         full_dom_pts;               // number of points in full domain in case a subset is taken
    vector<real_t>      min;                        // minimum corner of domain
    vector<real_t>      max;                        // maximum corner of domain
    vector<real_t>      s;                          // scaling factor for each variable or any other usage
    real_t              r;                          // x-y rotation of domain or any other usage
    vector<real_t>      f;                          // frequency multiplier for each variable or any other usage
    real_t              t;                          // waviness of domain edges or any other usage
    real_t              n;                          // noise factor [0.0 - 1.0]
    string              infile;                     // input filename
    bool                multiblock;                 // multiblock domain, get bounds from block
    bool                structured;                 // input data lies on unstructured grid
    int                 rand_seed;                  // seed for generating random data. -1: no randomization, 0: choose seed at random
};

// block
template <typename T>
struct Block : public BlockBase<T>
{
    using Base = BlockBase<T>;
    using Base::dom_dim;
    using Base::pt_dim;
    using Base::core_mins;
    using Base::core_maxs;
    using Base::bounds_mins;
    using Base::bounds_maxs;
    using Base::overlaps;
    using Base::input;

    void my_decode_point(
            const VectorX<T>&                   param,          // parameters of point to decode
            Eigen::Ref<VectorX<T>>              cpt)            // (output) decoded point
                                                                // using Eigen::Ref instead of C++ reference so that pybind11 can pass by reference
    {
        // geometry
        std::cerr << "++++++++dom_dim: " << dom_dim << std::endl;
        VectorX<T> geom_cpt(dom_dim);
        this->mfa->DecodePt(*(this->geometry.mfa_data), param, geom_cpt);
        for (auto i = 0; i < dom_dim; i++)
            cpt(i) = geom_cpt(i);

        // science variables
        VectorX<T> var_cpt(1);                                  // each variable is a scalar
        std::cerr << "++++++++vars.size(): " << this->vars.size() << std::endl;
        for (auto i = 0; i < this->vars.size(); i++)
        {
            this->mfa->DecodePt(*(this->vars[i].mfa_data), param, var_cpt);
            cpt(dom_dim + i) = var_cpt(0);
        }
    }


    static
        void* create()              { return mfa::create<Block>(); }

    static
        void destroy(void* b)       { mfa::destroy<Block>(b); }

    static
        void add(                                   // add the block to the decomposition
            int                 gid,                // block global id
            const Bounds<T>&    core,               // block bounds without any ghost added
            const Bounds<T>&    bounds,             // block bounds including any ghost region added
            const Bounds<T>&    domain,             // global data bounds
            const RCLink<T>&    link,               // neighborhood
            diy::Master&        master,             // diy master
            int                 dom_dim,            // domain dimensionality
            int                 pt_dim,             // point dimensionality
            T                   ghost_factor = 0.0) // amount of ghost zone overlap as a factor of block size (0.0 - 1.0)
    {
        mfa::add<Block, T>(gid, core, bounds, domain, link, master, dom_dim, pt_dim, ghost_factor);
    }

    static
        void save(const void* b_, diy::BinaryBuffer& bb)    { mfa::save<Block, T>(b_, bb); }
    static
        void load(void* b_, diy::BinaryBuffer& bb)          { mfa::load<Block, T>(b_, bb); }

    // NB: Because BlockBase, the parent of Block, is templated, the C++ compiler requires
    // access to members in BlockBase to be preceded by "this->".
    // Otherwise, the compiler can't be sure that the member exists. [Myers Effective C++, item 43]
    // This is annoying but unavoidable.

    // evaluate sine function
    T sine(VectorX<T>&  domain_pt,
           DomainArgs&  args,
           int          k)                  // current science variable
    {
        DomainArgs* a = &args;
        T retval = 1.0;
        for (auto i = 0; i < this->dom_dim; i++)
            retval *= sin(domain_pt(i) * a->f[k]);
        retval *= a->s[k];

        return retval;
    }

    // evaluate sinc function
    T sinc(VectorX<T>&  domain_pt,
           DomainArgs&  args,
           int          k)                  // current science variable
    {
        DomainArgs* a = &args;
        T retval = 1.0;
        for (auto i = 0; i < this->dom_dim; i++)
        {
            if (domain_pt(i) != 0.0)
                retval *= (sin(domain_pt(i) * a->f[k] ) / domain_pt(i));
        }
        retval *= a->s[k];

        return retval;
    }

    // evaluate 2d poly-sinc function version 1
    T polysinc1(VectorX<T>& domain_pt,
                DomainArgs& args)
    {
        // only for 2d
        if (this->dom_dim != 2)
        {
            fprintf(stderr, "Polysinc 1 function only defined for 2d. Aborting.\n");
            exit(0);
        }
        T x = domain_pt(0);
        T y = domain_pt(1);
        T a = (x + 1) * (x + 1) + (y - 1) * (y - 1);
        T b = (x - 1) * (x - 1) + (y + 1) * (y + 1);
        T a1 = (a == 0.0 ? 1.0 : sin(a) / a);
        T b1 = (b == 0.0 ? 1.0 : sin(b) / b);
        return args.s[0] * (a1 + b1);
    }

    // evaluate 2d poly-sinc function version 2
    T polysinc2(VectorX<T>& domain_pt,
                DomainArgs& args)
    {
        // only for 2d
        if (this->dom_dim != 2)
        {
            fprintf(stderr, "Polysinc 2 function only defined for 2d. Aborting.\n");
            exit(0);
        }
        T x = domain_pt(0);
        T y = domain_pt(1);
        T a = x * x + y * y;
        T b = 2 * (x - 2) * (x - 2) + (y + 2) * (y + 2);
        T a1 = (a == 0.0 ? 1.0 : sin(a) / a);
        T b1 = (b == 0.0 ? 1.0 : sin(b) / b);
        return args.s[0] * (a1 + b1);
    }

    // evaluate Marschner-Lobb function [Marschner and Lobb, IEEE VIS, 1994]
    // only for a 3d domain
    // using args f[0] and s[0] for f_M and alpha, respectively, in the paper
    T ml(VectorX<T>&  domain_pt,
           DomainArgs&  args)
    {
        DomainArgs* a   = &args;
        T& fm           = a->f[0];
        T& alpha        = a->s[0];
//         T fm = 6.0;
//         T alpha = 0.25;
        T& x            = domain_pt(0);
        T& y            = domain_pt(1);
        T& z            = domain_pt(2);

        // T rad       = sqrt(x * x + y * y + z * z);
        // T rho       = cos(2 * M_PI * fm * cos(M_PI * rad / 2.0));
        // T retval    = (1.0 - sin(M_PI * z / 2.0) + alpha * (1.0 + rho * sqrt(x * x + y * y))) / (2 * (1.0 + alpha));

        T rad       = sqrt(x * x + y * y );
        T rho       = cos(2 * M_PI * fm * cos(M_PI * rad / 2.0));
        T retval    = (1.0 - sin(M_PI * z / 2.0) + alpha * (1.0 + rho)) / (2 * (1.0 + alpha));

        return retval;
    }

    // evaluate f16 function
    T f16(VectorX<T>&   domain_pt)
    {
        T retval =
            (pow(domain_pt(0), 4)                        +
             pow(domain_pt(1), 4)                        +
             pow(domain_pt(0), 2) * pow(domain_pt(1), 2) +
             domain_pt(0) * domain_pt(1)                 ) /
            (pow(domain_pt(0), 3)                        +
             pow(domain_pt(1), 3)                        +
             4                                           );

        return retval;
    }

    // evaluate f17 function
    T f17(VectorX<T>&   domain_pt)
    {
        T E         = domain_pt(0);
        T G         = domain_pt(1);
        T M         = domain_pt(2);
        T gamma     = sqrt(M * M * (M * M + G * G));
        T kprop     = (2.0 * sqrt(2.0) * M * G * gamma ) / (M_PI * sqrt(M * M + gamma));
        T retval    = kprop / ((E * E - M * M) * (E * E - M * M) + M * M * G * G);

        return retval;
    }

    // evaluate f18 function
    T f18(VectorX<T>&   domain_pt)
    {
        T x1        = domain_pt(0);
        T x2        = domain_pt(1);
        T x3        = domain_pt(2);
        T x4        = domain_pt(3);
        T retval    = (atanh(x1) + atanh(x2) + atanh(x3) + atanh(x4)) / ((pow(x1, 2) - 1) * pow(x2, -1));

        return retval;
    }

    void generate_analytical_data(
            const diy::Master::ProxyWithLink&   cp,
            string&                             fun,
            DomainArgs&                         args)
    {
        if (args.rand_seed >= 0)  // random point cloud
        {
            cout << "Generating data on random point cloud for function: " << fun << endl;

            if (args.structured)
            {
                cerr << "ERROR: Cannot perform structured encoding of random point cloud" << endl;
                exit(1);
            }

            // Prep a few more domain arguments which are used by generate_random_analytical_data
            args.model_dims.resize(2);
            args.model_dims[0] = dom_dim;
            args.model_dims[1] = 1;

            args.tot_ndom_pts = 1;
            for (size_t k = 0; k < dom_dim; k++)
            {
                args.tot_ndom_pts *= args.ndom_pts[k];
            }

            // create unsigned conversion of seed
            // note: seed is always >= 0 in this code block
            unsigned useed = (unsigned)args.rand_seed;
            generate_random_analytical_data(cp, fun, args, useed);
        }
        else    // structured grid of points
        {
            cout << "Generating data on structured grid for function: " << fun << endl;
            generate_rectilinear_analytical_data(cp, fun, args);
        }
    }
    

    // synthetic analytic (scalar) data, sampled on unstructured point cloud
    // when seed = 0, we choose a time-dependent seed for the random number generator
    void generate_random_analytical_data(
            const diy::Master::ProxyWithLink&   cp,
            string&                             fun,
            DomainArgs&                         args,
            unsigned int                        seed)
    {
        assert(!args.structured);

        DomainArgs* a = &args;

        // Prepare containers
        size_t nvars = a->model_dims.size()-1;
        size_t geom_dim = a->model_dims[0];
        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);

        // Assign min/max dimensions for each model
        this->geometry.min_dim = 0;
        this->geometry.max_dim = geom_dim - 1;
        this->vars[0].min_dim = a->model_dims[0];
        this->vars[0].max_dim = this->vars[0].min_dim + a->model_dims[0];
        for (size_t n = 1; n < nvars; n++)
        {
            this->vars[n].min_dim = this->vars[n-1].max_dim + 1;
            this->vars[n].max_dim = this->vars[n].min_dim + a->model_dims[n];
        }

        // Set block bounds (if not already done by DIY)
        if (!a->multiblock)
        {
            bounds_mins.resize(pt_dim);
            bounds_maxs.resize(pt_dim);
            core_mins.resize(geom_dim);
            core_maxs.resize(geom_dim);
            for (int i = 0; i < geom_dim; i++)
            {
                bounds_mins(i)  = a->min[i];
                bounds_maxs(i)  = a->max[i];
                core_mins(i)    = a->min[i];
                core_maxs(i)    = a->max[i];
            }
        }

        // decide overlap in each direction; they should be symmetric for neighbors
        // so if block a overlaps block b, block b overlaps a the same area
        for (size_t k = 0; k < geom_dim; k++)
        {
            overlaps(k) = fabs(core_mins(k) - bounds_mins(k));
            T m2 = fabs(bounds_maxs(k) - core_maxs(k));
            if (m2 > overlaps(k))
                overlaps(k) = m2;
        }


        // Create input data set and add to block
        input = new mfa::PointSet<T>(dom_dim, pt_dim, a->tot_ndom_pts);
        input->set_bounds(core_mins, core_maxs);

        // Choose a system-dependent seed if seed==0
        if (seed == 0)
            seed = chrono::system_clock::now().time_since_epoch().count();

        std::default_random_engine df_gen(seed);
        std::uniform_real_distribution<double> u_dist(0.0, 1.0);

        // Fill domain with randomly distributed points
        size_t nvoids = 3;
        double keep_frac = 1.0/5.0;
        double radii_frac = 1.0/10.0;   // fraction of domain width to set as void radius
        VectorX<T> radii(nvoids);
        MatrixX<T> centers(geom_dim, nvoids);
        for (size_t nv = 0; nv < nvoids; nv++) // Randomly generate the centers of each void
        {
            for (size_t k = 0; k < geom_dim; k++)
            {
                centers(k,nv) = input->dom_mins(k) + u_dist(df_gen) * (input->dom_maxs(k) - input->dom_mins(k));
            }

            radii(nv) = radii_frac * (input->dom_maxs - input->dom_mins).minCoeff();
        }

        for (size_t j = 0; j < input->domain.rows(); j++)
        {

            VectorX<T> candidate_pt(geom_dim);

            bool keep = true;
            do
            {
                // Generate a random point
                for (size_t k = 0; k < geom_dim; k++)
                {
                    // input->domain(j, k) = input->dom_mins(k) + u_dist(df_gen) * (input->dom_maxs(k) - input->dom_mins(k));
                    candidate_pt(k) = input->dom_mins(k) + u_dist(df_gen) * (input->dom_maxs(k) - input->dom_mins(k));
                }

                // Consider discarding point if within a certain radius of a void
                for (size_t nv = 0; nv < nvoids; nv++)
                {
                    if ((candidate_pt - centers.col(nv)).norm() < radii(nv))
                    {
                        keep = false;
                        break;
                    }
                }

                // Keep this point anyway a certain fraction of the time
                if (keep == false)
                {
                    if (u_dist(df_gen) <= keep_frac)
                        keep = true;
                }
            } while (!keep);    

            // Add point to Input
            for (size_t k = 0; k < geom_dim; k++)
            {
                input->domain(j,k) = candidate_pt(k);
            }

            VectorX<T> dom_pt = input->domain.block(j, 0, 1, geom_dim).transpose();
            T retval;
            for (size_t n = 0; n < nvars; n++)        // for all science variables
            {
                if (fun == "sine")
                    retval = sine(dom_pt, args, n);
                if (fun == "sinc")
                    retval = sinc(dom_pt, args, n);
                if (fun == "psinc1")
                    retval = polysinc1(dom_pt, args);
                if (fun == "psinc2")
                    retval = polysinc2(dom_pt, args);
                if (fun == "ml")
                {
                    if (this->dom_dim != 3)
                    {
                        fprintf(stderr, "Error: Marschner-Lobb function is only defined for a 3d domain.\n");
                        exit(0);
                    }
                    retval = ml(dom_pt, args);
                }
                if (fun == "f16")
                    retval = f16(dom_pt);
                if (fun == "f17")
                    retval = f17(dom_pt);
                if (fun == "f18")
                    retval = f18(dom_pt);
                input->domain(j, geom_dim + n) = retval;

                if (j == 0 || input->domain(j, geom_dim + n) > bounds_maxs(geom_dim + n))
                    bounds_maxs(geom_dim + n) = input->domain(j, geom_dim + n);
                if (j == 0 || input->domain(j, geom_dim + n) < bounds_mins(geom_dim + n))
                    bounds_mins(geom_dim + n) = input->domain(j, geom_dim + n);
            }      
        }

        input->init_params(core_mins, core_maxs);     // Set explicit bounding box for parameter space
        this->mfa = new mfa::MFA<T>(dom_dim);

        // extents
        fprintf(stderr, "gid = %d\n", cp.gid());
        cerr << "core_mins:\n" << core_mins << endl;
        cerr << "core_maxs:\n" << core_maxs << endl;
        cerr << "bounds_mins:\n" << bounds_mins << endl;
        cerr << "bounds_maxs:\n" << bounds_maxs << endl;
    }

    // Creates a synthetic dataset on a rectilinear grid of points
    // This grid can be treated as EITHER a "structured" or "unstructured"
    // PointSet by setting the args.structured field appropriately
    void generate_rectilinear_analytical_data(
            const diy::Master::ProxyWithLink&   cp,
            string&                             fun,        // function to evaluate
            DomainArgs&                         args)
    {
        DomainArgs* a   = &args;

        // TODO: This assumes that dom_dim = dimension of ambient geometry.
        //       Not always true, can model a 2d surface in 3d, e.g.
        //       Also assumes each var is scalar
        int nvars       = this->pt_dim - this->dom_dim;             // number of science variables

        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);
        // int tot_ndom_pts    = 1;
        this->geometry.min_dim = 0;
        this->geometry.max_dim = this->dom_dim - 1;  // TODO fix dom_dim assumption, see above
        for (int j = 0; j < nvars; j++)
        {
            this->vars[j].min_dim = this->dom_dim + j;
            this->vars[j].max_dim = this->vars[j].min_dim;
        }
        VectorXi ndom_pts(this->dom_dim);
        for (int i = 0; i < this->dom_dim; i++)
            ndom_pts(i) = a->ndom_pts[i];

        // get local block bounds
        // if single block, they are passed in args
        // if multiblock, they were decomposed by diy and are already in the block's bounds_mins, maxs
        if (!a->multiblock)
        {
            this->bounds_mins.resize(this->pt_dim);
            this->bounds_maxs.resize(this->pt_dim);
            this->core_mins.resize(this->dom_dim);
            this->core_maxs.resize(this->dom_dim);
            for (int i = 0; i < this->dom_dim; i++)
            {
                this->bounds_mins(i)  = a->min[i];
                this->bounds_maxs(i)  = a->max[i];
                this->core_mins(i)    = a->min[i];
                this->core_maxs(i)    = a->max[i];
            }
            this->bounds_mins(dom_dim) = numeric_limits<T>::min();
            this->bounds_maxs(dom_dim) = numeric_limits<T>::max();
        }

        // adjust number of domain points and starting domain point for ghost
        VectorX<T> d(this->dom_dim);               // step in domain points in each dimension
        VectorX<T> p0(this->dom_dim);              // starting point in each dimension
        int nghost_pts;                         // number of ghost points in current dimension
        for (int i = 0; i < this->dom_dim; i++)
        {
            d(i) = (this->core_maxs(i) - this->core_mins(i)) / (ndom_pts(i) - 1);
            // min direction
            nghost_pts = floor((this->core_mins(i) - this->bounds_mins(i)) / d(i));
            ndom_pts(i) += nghost_pts;
            p0(i) = this->core_mins(i) - nghost_pts * d(i);
            // max direction
            nghost_pts = floor((this->bounds_maxs(i) - this->core_maxs(i)) / d(i));
            ndom_pts(i) += nghost_pts;
            // tot_ndom_pts *= ndom_pts(i);

            // decide overlap in each direction; they should be symmetric for neighbors
            // so if block a overlaps block b, block b overlaps a the same area
            this->overlaps(i) = fabs(this->core_mins(i) - this->bounds_mins(i));
            T m2 = fabs(this->bounds_maxs(i) - this->core_maxs(i));
            if (m2 > this->overlaps(i))
                this->overlaps(i) = m2;
        }

        if (args.structured)
            input = new mfa::PointSet<T>(dom_dim, pt_dim, ndom_pts.prod(), ndom_pts);
        else
            input = new mfa::PointSet<T>(dom_dim, pt_dim, ndom_pts.prod());

        // assign values to the domain (geometry)
        mfa::VolIterator vol_it(ndom_pts);
        // current index of domain point in each dim, initialized to 0s
        // flattened loop over all the points in a domain
        while (!vol_it.done())
        {
            int j = (int)vol_it.cur_iter();
            // compute geometry coordinates of domain point
            for (auto i = 0; i < this->dom_dim; i++)
                input->domain(j, i) = p0(i) + vol_it.idx_dim(i) * d(i);

            vol_it.incr_iter();
        }

        // normal distribution for generating noise
        default_random_engine generator;
        normal_distribution<double> distribution(0.0, 1.0);

        // assign values to the range (science variables)
        VectorX<T> dom_pt(this->dom_dim); // dom_dim = 3

        // std::cout << "-----------------rows: " << input->domain.rows() << std::endl;
        // ofstream ml_file("ml.bin", ios::out | ios::binary);
        // std::cout << input->domain.rows() << std::endl; 
        // double *my_values = new double[input->domain.rows()];
        // int skip = 1000000;

        for (int j = 0; j < input->domain.rows(); j++) // rows = sample size ^ 3
        {
            dom_pt = input->domain.block(j, 0, 1, this->dom_dim).transpose();
            T retval;
            // if (j%skip == 0) {
            //     std::cout << j << std::endl;
            // }
            for (auto k = 0; k < nvars; k++)        // for all science variables
            {
                if (fun == "sine")
                    retval = sine(dom_pt, args, k);
                if (fun == "sinc")
                    retval = sinc(dom_pt, args, k);
                if (fun == "psinc1")
                    retval = polysinc1(dom_pt, args);
                if (fun == "psinc2")
                    retval = polysinc2(dom_pt, args);
                if (fun == "ml")
                {
                    if (this->dom_dim != 3)
                    {
                        fprintf(stderr, "Error: Marschner-Lobb function is only defined for a 3d domain.\n");
                        exit(0);
                    }
                    retval = ml(dom_pt, args);
                    // my_values[j] = (double)retval;
                }
                if (fun == "f16")
                    retval = f16(dom_pt);
                if (fun == "f17")
                    retval = f17(dom_pt);
                if (fun == "f18")
                    retval = f18(dom_pt);
                input->domain(j, this->dom_dim + k) = retval;
            }

            // add some noise
            double noise = distribution(generator);
            input->domain(j, this->dom_dim) *= (1.0 + a->n * noise);

            if (j == 0 || input->domain(j, this->dom_dim) > this->bounds_maxs(this->dom_dim))
                this->bounds_maxs(this->dom_dim) = input->domain(j, this->dom_dim);
            if (j == 0 || input->domain(j, this->dom_dim) < this->bounds_mins(this->dom_dim))
                this->bounds_mins(this->dom_dim) = input->domain(j, this->dom_dim);
        }
        // ml_file.write((char *)my_values, 8 * input->domain.rows());
        // ml_file.close();
        // exit (EXIT_FAILURE);

        // optional wavy domain
        if (a->t && this->pt_dim >= 3)
        {
            for (auto j = 0; j < input->domain.rows(); j++)
            {
                real_t x = input->domain(j, 0);
                real_t y = input->domain(j, 1);
                input->domain(j, 0) += a->t * sin(y);
                input->domain(j, 1) += a->t * sin(x);
                if (j == 0 || input->domain(j, 0) < this->bounds_mins(0))
                    this->bounds_mins(0) = input->domain(j, 0);
                if (j == 0 || input->domain(j, 1) < this->bounds_mins(1))
                    this->bounds_mins(1) = input->domain(j, 1);
                if (j == 0 || input->domain(j, 0) > this->bounds_maxs(0))
                    this->bounds_maxs(0) = input->domain(j, 0);
                if (j == 0 || input->domain(j, 1) > this->bounds_maxs(1))
                    this->bounds_maxs(1) = input->domain(j, 1);
            }
        }

        // optional rotation of the domain
        if (a->r && this->pt_dim >= 3)
        {
            for (auto j = 0; j < input->domain.rows(); j++)
            {
                real_t x = input->domain(j, 0);
                real_t y = input->domain(j, 1);
                input->domain(j, 0) = x * cos(a->r) - y * sin(a->r);
                input->domain(j, 1) = x * sin(a->r) + y * cos(a->r);
                if (j == 0 || input->domain(j, 0) < this->bounds_mins(0))
                    this->bounds_mins(0) = input->domain(j, 0);
                if (j == 0 || input->domain(j, 1) < this->bounds_mins(1))
                    this->bounds_mins(1) = input->domain(j, 1);
                if (j == 0 || input->domain(j, 0) > this->bounds_maxs(0))
                    this->bounds_maxs(0) = input->domain(j, 0);
                if (j == 0 || input->domain(j, 1) > this->bounds_maxs(1))
                    this->bounds_maxs(1) = input->domain(j, 1);
            }
        }

        // map_dir is used in blending discrete, but because we need to aggregate the discrete logic, we have to use
        // it even for continuous bounds, so in analytical data
        // this is used in s3d data because the actual domain dim is derived
        for (int k = 0; k < this->dom_dim; k++)
            this->map_dir.push_back(k);

        input->init_params();
        this->mfa = new mfa::MFA<T>(this->dom_dim);

        // extents
        fprintf(stderr, "gid = %d\n", cp.gid());
        cerr << "core_mins:\n" << this->core_mins << endl;
        cerr << "core_maxs:\n" << this->core_maxs << endl;
        cerr << "bounds_mins:\n" << this->bounds_mins << endl;
        cerr << "bounds_maxs:\n" << this->bounds_maxs << endl;

//         cerr << "ndom_pts:\n" << ndom_pts << "\n" << endl;
//         cerr << "domain:\n" << this->domain << endl;
    }

    // read a floating point 3d vector dataset and take one 1-d curve out of the middle of it
    // f = (x, velocity magnitude)
    void read_1d_slice_3d_vector_data(
            const       diy::Master::ProxyWithLink& cp,
            DomainArgs& args)
    {   
        DomainArgs* a = &args;
        int tot_ndom_pts = 1;
        this->geometry.min_dim = 0;
        this->geometry.max_dim = this->dom_dim - 1;
        int nvars = 1;
        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);
        this->vars[0].min_dim = this->dom_dim;
        this->vars[0].max_dim = this->vars[0].min_dim;
        VectorXi ndom_pts(this->dom_dim);
        this->bounds_mins.resize(this->pt_dim);
        this->bounds_maxs.resize(this->pt_dim);
        for (int i = 0; i < this->dom_dim; i++)
        {
            ndom_pts(i)                     =  a->ndom_pts[i];
            tot_ndom_pts                    *= ndom_pts(i);
        }
        vector<float> vel(3 * tot_ndom_pts);

        // Construct point set to contain input
        if (args.structured)
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts, ndom_pts);
        else
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts);

        // rest is hard-coded for 1d

        // open file and seek to a slice in the center
        FILE *fd = fopen(a->infile.c_str(), "r");
        assert(fd);
        fseek(fd, (a->ndom_pts[0] * a->ndom_pts[1] * a->ndom_pts[2] / 2 + a->ndom_pts[0] * a->ndom_pts[1] / 2) * 12, SEEK_SET);

        // read all three components of velocity and compute magnitude
        if (!fread(&vel[0], sizeof(float), tot_ndom_pts * 3, fd))
        {
            fprintf(stderr, "Error: unable to read file\n");
            exit(0);
        }
        for (size_t i = 0; i < vel.size() / 3; i++)
        {
            input->domain(i, 1) = sqrt(vel[3 * i    ] * vel[3 * i    ] +
                    vel[3 * i + 1] * vel[3 * i + 1] +
                    vel[3 * i + 2] * vel[3 * i + 2]);
            // fprintf(stderr, "vel [%.3f %.3f %.3f] mag %.3f\n",
            //         vel[3 * i], vel[3 * i + 1], vel[3 * i + 2], range[i]);
        }

        // find extent of range
        for (size_t i = 0; i < (size_t)input->domain.rows(); i++)
        {
            if (i == 0 || input->domain(i, 1) < bounds_mins(1))
                bounds_mins(1) = input->domain(i, 1);
            if (i == 0 || input->domain(i, 1) > bounds_maxs(1))
                bounds_maxs(1) = input->domain(i, 1);
        }

        // set domain values (just equal to i, j; ie, dx, dy = 1, 1)
        int n = 0;
        for (size_t i = 0; i < (size_t)(ndom_pts(0)); i++)
        {
            input->domain(n, 0) = i;
            n++;
        }

        // extents
        bounds_mins(0) = 0.0;
        bounds_maxs(0) = input->domain(tot_ndom_pts - 1, 0);
        core_mins.resize(dom_dim);
        core_maxs.resize(dom_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            core_mins(i) = bounds_mins(i);
            core_maxs(i) = bounds_maxs(i);
        }

        input->init_params();
        this->mfa = new mfa::MFA<T>(dom_dim);

        // debug
        cerr << "domain extent:\n min\n" << bounds_mins << "\nmax\n" << bounds_maxs << endl;
    }

    // read a floating point 3d vector dataset and take one 2-d surface out of the middle of it
    // f = (x, y, velocity magnitude)
    void read_2d_slice_3d_vector_data(
            const       diy::Master::ProxyWithLink& cp,
            DomainArgs& args)
    {
        DomainArgs* a = &args;
        int tot_ndom_pts = 1;
        this->geometry.min_dim = 0;
        this->geometry.max_dim = dom_dim - 1;
        int nvars = 1;
        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);
        this->vars[0].min_dim = dom_dim;
        this->vars[0].max_dim = this->vars[0].min_dim;
        VectorXi ndom_pts(dom_dim);
        this->bounds_mins.resize(pt_dim);
        this->bounds_maxs.resize(pt_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            ndom_pts(i)                     =  a->ndom_pts[i];
            tot_ndom_pts                    *= ndom_pts(i);
        }
        vector<float> vel(3 * tot_ndom_pts);

        // Construct point set to contain input
        if (args.structured)
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts, ndom_pts);
        else
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts);

        // rest is hard-coded for 2d

        // open file and seek to a slice in the center
        FILE *fd = fopen(a->infile.c_str(), "r");
        assert(fd);
        // middle plane in z, offset = full x,y range * 1/2 z range
        fseek(fd, (a->ndom_pts[0] * a->ndom_pts[1] * a->ndom_pts[2] / 2) * 12, SEEK_SET);

        // read all three components of velocity and compute magnitude
        if (!fread(&vel[0], sizeof(float), tot_ndom_pts * 3, fd))
        {
            fprintf(stderr, "Error: unable to read file\n");
            exit(0);
        }
        for (size_t i = 0; i < vel.size() / 3; i++)
        {
            input->domain(i, 2) = sqrt(vel[3 * i    ] * vel[3 * i    ] +
                    vel[3 * i + 1] * vel[3 * i + 1] +
                    vel[3 * i + 2] * vel[3 * i + 2]);
//              fprintf(stderr, "vel [%.3f %.3f %.3f]\n",
//                      vel[3 * i], vel[3 * i + 1], vel[3 * i + 2]);
        }

        // find extent of range
        for (size_t i = 0; i < (size_t)input->domain.rows(); i++)
        {
            if (i == 0 || input->domain(i, 2) < bounds_mins(2))
                bounds_mins(2) = input->domain(i, 2);
            if (i == 0 || input->domain(i, 2) > bounds_maxs(2))
                bounds_maxs(2) = input->domain(i, 2);
        }

        // set domain values (just equal to i, j; ie, dx, dy = 1, 1)
        int n = 0;
        for (size_t j = 0; j < (size_t)(ndom_pts(1)); j++)
            for (size_t i = 0; i < (size_t)(ndom_pts(0)); i++)
            {
                input->domain(n, 0) = i;
                input->domain(n, 1) = j;
                n++;
            }

        // extents
        bounds_mins(0) = 0.0;
        bounds_mins(1) = 0.0;
        bounds_maxs(0) = input->domain(tot_ndom_pts - 1, 0);
        bounds_maxs(1) = input->domain(tot_ndom_pts - 1, 1);
        core_mins.resize(dom_dim);
        core_maxs.resize(dom_dim);
        for (int i = 0; i < this->dom_dim; i++)
        {
            core_mins(i) = bounds_mins(i);
            core_maxs(i) = bounds_maxs(i);
        }

        input->init_params();
        this->mfa = new mfa::MFA<T>(dom_dim);

        // debug
        cerr << "domain extent:\n min\n" << bounds_mins << "\nmax\n" << bounds_maxs << endl;
    }

    // read a floating point 3d vector dataset and take one 2d (parallel to x-y plane) subset
    // f = (x, y, velocity magnitude)
    void read_2d_subset_3d_vector_data(
            const       diy::Master::ProxyWithLink& cp,
            DomainArgs& args)
    {
        DomainArgs* a = &args;
        int tot_ndom_pts = 1;
        this->geometry.min_dim = 0;
        this->geometry.max_dim = dom_dim - 1;
        int nvars = 1;
        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);
        this->vars[0].min_dim = dom_dim;
        this->vars[0].max_dim = this->vars[0].min_dim;
        VectorXi ndom_pts(dom_dim);
        bounds_mins.resize(pt_dim);
        bounds_maxs.resize(pt_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            ndom_pts(i)     =  a->ndom_pts[i];
            tot_ndom_pts    *= ndom_pts(i);
        }
        vector<float> vel(a->full_dom_pts[0] * a->full_dom_pts[1] * 3);

        // Construct point set to contain input
        if (args.structured)
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts, ndom_pts);
        else
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts);

        FILE *fd = fopen(a->infile.c_str(), "r");
        assert(fd);

        // rest is hard-coded for 2d

        // seek to start of desired full x-y plane
        size_t ofst = 0;                                                    // offset to seek to (in bytes)
        ofst += a->starts[2] * a->full_dom_pts[0] * a->full_dom_pts[1];     // z direction
        ofst *= 12;                                                         // 3 components * 4 bytes
        fseek(fd, ofst, SEEK_SET);

        // read all three components of velocity for the entire plane (not just the subset)
        if (!fread(&vel[0], sizeof(float), a->full_dom_pts[0] * a->full_dom_pts[1] * 3, fd))
        {
            fprintf(stderr, "Error: unable to read file\n");
            exit(0);
        }

        // compute velocity magnitude only for the points in the subset, dropping the rest
        size_t ijk[2] = {0, 0};                          // i,j,k indices of current point
        size_t n = 0;
        for (size_t i = 0; i < vel.size() / 3; i++)
        {
            // is the point in the subset?
            bool keep = true;
            if (ijk[0] < a->starts[0] || ijk[0] >= a->starts[0] + a->ndom_pts[0] ||
                    ijk[1] < a->starts[1] || ijk[1] >= a->starts[1] + a->ndom_pts[1])
                keep = false;

            // debug
            //                 fprintf(stderr, "i=%ld ijk=[%ld %ld] keep=%d\n", i, ijk[0], ijk[1], keep);

            if (keep)
            {
                input->domain(n, 0) = ijk[0];                  // domain is just i,j
                input->domain(n, 1) = ijk[1];
                // range (function value) is magnitude of velocity
                input->domain(n, 2) = sqrt(vel[3 * i    ] * vel[3 * i    ] +
                        vel[3 * i + 1] * vel[3 * i + 1] +
                        vel[3 * i + 2] * vel[3 * i + 2]);
                n++;
                // fprintf(stderr, "vel [%.3f %.3f %.3f] mag %.3f\n",
                //         vel[3 * i], vel[3 * i + 1], vel[3 * i + 2], range[i]);
            }

            // increment ijk
            if (ijk[0] == a->full_dom_pts[0] - 1)
            {
                ijk[0] = 0;
                ijk[1]++;
            }
            else
                ijk[0]++;
        }

        // find extent of range
        for (size_t i = 0; i < (size_t)input->domain.rows(); i++)
        {
            if (i == 0 || input->domain(i, 2) < bounds_mins(2))
                bounds_mins(2) = input->domain(i, 2);
            if (i == 0 || input->domain(i, 2) > bounds_maxs(2))
                bounds_maxs(2) = input->domain(i, 2);
        }

        // extent of domain is just lower left and upper right corner, which in row-major order
        // is the first point and the last point
        bounds_mins(0) = input->domain(0, 0);
        bounds_mins(1) = input->domain(0, 1);
        bounds_maxs(0) = input->domain(tot_ndom_pts - 1, 0);
        bounds_maxs(1) = input->domain(tot_ndom_pts - 1, 1);
        core_mins.resize(dom_dim);
        core_maxs.resize(dom_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            core_mins(i) = bounds_mins(i);
            core_maxs(i) = bounds_maxs(i);
        }

        input->init_params();
        this->mfa = new mfa::MFA<T>(dom_dim);

        // debug
        cerr << "domain extent:\n min\n" << bounds_mins << "\nmax\n" << bounds_maxs << endl;
    }

    // read a floating point 3d vector dataset
    // f = (x, y, z, velocity magnitude)
    void read_3d_vector_data(
            const       diy::Master::ProxyWithLink& cp,
            DomainArgs& args)
    {
        DomainArgs* a = &args;
        int tot_ndom_pts = 1;
        this->geometry.min_dim = 0;
        this->geometry.max_dim = dom_dim - 1;
        int nvars = 1;
        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);
        this->vars[0].min_dim = dom_dim;
        this->vars[0].max_dim = this->vars[0].min_dim;
        VectorXi ndom_pts(dom_dim);
        this->bounds_mins.resize(pt_dim);
        this->bounds_maxs.resize(pt_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            ndom_pts(i)                     =  a->ndom_pts[i];
            tot_ndom_pts                    *= ndom_pts(i);
        }

        // Construct point set to contain input
        if (args.structured)
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts, ndom_pts);
        else
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts);

        vector<float> vel(3 * tot_ndom_pts);

        FILE *fd = fopen(a->infile.c_str(), "r");
        assert(fd);

        // read all three components of velocity and compute magnitude
        if (!fread(&vel[0], sizeof(float), tot_ndom_pts * 3, fd))
        {
            fprintf(stderr, "Error: unable to read file\n");
            exit(0);
        }
        for (size_t i = 0; i < vel.size() / 3; i++)
        {
            input->domain(i, 3) = sqrt(vel[3 * i    ] * vel[3 * i    ] +
                    vel[3 * i + 1] * vel[3 * i + 1] +
                    vel[3 * i + 2] * vel[3 * i + 2]);
//             if (i < 1000)
//              fprintf(stderr, "vel [%.3f %.3f %.3f]\n",
//                      vel[3 * i], vel[3 * i + 1], vel[3 * i + 2]);
        }
        std::cout << "xxx val[0]: " << vel[0] << std::endl;
        std::cout << "xxx val[0]: " << vel.at(0) << std::endl;
        std::cout << "xxx val[1]: " << vel[1] << std::endl;
        std::cout << "xxx val[1]: " << vel.at(1) << std::endl;

        // rest is hard-coded for 3d

        // find extent of range
        for (size_t i = 0; i < (size_t)input->domain.rows(); i++)
        {
            if (i == 0 || input->domain(i, 3) < bounds_mins(3))
                bounds_mins(3) = input->domain(i, 3);
            if (i == 0 || input->domain(i, 3) > bounds_maxs(3))
                bounds_maxs(3) = input->domain(i, 3);
        }

        // set domain values (just equal to i, j; ie, dx, dy = 1, 1)
        int n = 0;
        for (size_t k = 0; k < (size_t)(ndom_pts(2)); k++)
            for (size_t j = 0; j < (size_t)(ndom_pts(1)); j++)
                for (size_t i = 0; i < (size_t)(ndom_pts(0)); i++)
                {
                    input->domain(n, 0) = i;
                    input->domain(n, 1) = j;
                    input->domain(n, 2) = k;
                    n++;
                }

        // extents
        bounds_mins(0) = 0.0;
        bounds_mins(1) = 0.0;
        bounds_mins(2) = 0.0;
        bounds_maxs(0) = input->domain(tot_ndom_pts - 1, 0);
        bounds_maxs(1) = input->domain(tot_ndom_pts - 1, 1);
        bounds_maxs(2) = input->domain(tot_ndom_pts - 1, 2);
        core_mins.resize(dom_dim);
        core_maxs.resize(dom_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            core_mins(i) = bounds_mins(i);
            core_maxs(i) = bounds_maxs(i);
        }

        input->init_params();
        this->mfa = new mfa::MFA<T>(dom_dim);

        // debug
        cerr << "domain extent:\n min\n" << bounds_mins << "\nmax\n" << bounds_maxs << endl;
    }

    // read a floating point 3d vector dataset and take a 3d subset out of it
    // f = (x, y, z, velocity magnitude)
    void read_3d_subset_3d_vector_data(
            const       diy::Master::ProxyWithLink& cp,
            DomainArgs& args)
    {
        DomainArgs* a = &args;
        int tot_ndom_pts = 1;
        this->geometry.min_dim = 0;
        this->geometry.max_dim = dom_dim - 1;
        int nvars = 1;
        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);
        this->vars[0].min_dim = dom_dim;
        this->vars[0].max_dim = this->vars[0].min_dim;
        VectorXi ndom_pts(dom_dim);
        bounds_mins.resize(pt_dim);
        bounds_maxs.resize(pt_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            ndom_pts(i)     =  a->ndom_pts[i];
            tot_ndom_pts    *= ndom_pts(i);
        }

        // Construct point set to contain input
        if (args.structured)
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts, ndom_pts);
        else
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts);

        vector<float> vel(a->full_dom_pts[0] * a->full_dom_pts[1] * a->full_dom_pts[2] * 3);

        FILE *fd = fopen(a->infile.c_str(), "r");
        assert(fd);

        // rest is hard-coded for 3d

        // read all three components of velocity (not just the subset)
        if (!fread(&vel[0], sizeof(float), a->full_dom_pts[0] * a->full_dom_pts[1] * a->full_dom_pts[2] * 3, fd))
        {
            fprintf(stderr, "Error: unable to read file\n");
            exit(0);
        }

        // compute velocity magnitude only for the points in the subset, dropping the rest
        size_t ijk[3] = {0, 0, 0};                          // i,j,k indices of current point
        size_t n = 0;
        for (size_t i = 0; i < vel.size() / 3; i++)
        {
            // is the point in the subset?
            bool keep = true;
            if (ijk[0] < a->starts[0] || ijk[0] >= a->starts[0] + a->ndom_pts[0] ||
                    ijk[1] < a->starts[1] || ijk[1] >= a->starts[1] + a->ndom_pts[1] ||
                    ijk[2] < a->starts[2] || ijk[2] >= a->starts[2] + a->ndom_pts[2])
                keep = false;

            // debug
            //                 fprintf(stderr, "i=%ld ijk=[%ld %ld %ld] keep=%d\n", i, ijk[0], ijk[1], ijk[2], keep);

            if (keep)
            {
                input->domain(n, 0) = ijk[0];                  // domain is just i,j
                input->domain(n, 1) = ijk[1];
                input->domain(n, 2) = ijk[2];
                input->domain(n, 3) = sqrt(vel[3 * i    ] * vel[3 * i    ] +
                        vel[3 * i + 1] * vel[3 * i + 1] +
                        vel[3 * i + 2] * vel[3 * i + 2]);
                n++;
                // fprintf(stderr, "vel [%.3f %.3f %.3f] mag %.3f\n",
                //         vel[3 * i], vel[3 * i + 1], vel[3 * i + 2], range[i]);
            }

            // increment ijk
            if (ijk[0] == a->full_dom_pts[0] - 1)
            {
                ijk[0] = 0;
                if (ijk[1] == a->full_dom_pts[1] - 1)
                {
                    ijk[1] = 0;
                    ijk[2]++;
                }
                else
                    ijk[1]++;
            }
            else
                ijk[0]++;
        }

        // find extent of range
        for (size_t i = 0; i < (size_t)input->domain.rows(); i++)
        {
            if (i == 0 || input->domain(i, 3) < bounds_mins(3))
                bounds_mins(3) = input->domain(i, 3);
            if (i == 0 || input->domain(i, 3) > bounds_maxs(3))
                bounds_maxs(3) = input->domain(i, 3);
        }

        // extent of domain is just lower left and upper right corner, which in row-major order
        // is the first point and the last point
        bounds_mins(0) = input->domain(0, 0);
        bounds_mins(1) = input->domain(0, 1);
        bounds_mins(2) = input->domain(0, 2);
        bounds_maxs(0) = input->domain(tot_ndom_pts - 1, 0);
        bounds_maxs(1) = input->domain(tot_ndom_pts - 1, 1);
        bounds_maxs(2) = input->domain(tot_ndom_pts - 1, 2);
        core_mins.resize(dom_dim);
        core_maxs.resize(dom_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            core_mins(i) = bounds_mins(i);
            core_maxs(i) = bounds_maxs(i);
        }

        input->init_params();
        this->mfa = new mfa::MFA<T>(dom_dim);

        // debug
        cerr << "domain extent:\n min\n" << bounds_mins << "\nmax\n" << bounds_maxs << endl;
    }

    // read a floating point 2d scalar dataset
    // f = (x, y, value)
    void read_2d_scalar_data(
            const       diy::Master::ProxyWithLink& cp,
            DomainArgs& args)
    {
        DomainArgs* a = &args;
        int tot_ndom_pts = 1;
        this->geometry.min_dim = 0;
        this->geometry.max_dim = dom_dim - 1;
        int nvars = 1;
        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);
        this->vars[0].min_dim = dom_dim;
        this->vars[0].max_dim = this->vars[0].min_dim;
        VectorXi ndom_pts(dom_dim);
        bounds_mins.resize(pt_dim);
        bounds_maxs.resize(pt_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            ndom_pts(i)     =  a->ndom_pts[i];
            tot_ndom_pts    *= ndom_pts(i);
        }

        // Construct point set to contain input
        if (args.structured)
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts, ndom_pts);
        else
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts);

        vector<float> val(tot_ndom_pts);

        FILE *fd = fopen(a->infile.c_str(), "r");
        assert(fd);

        // read data values
        if (!fread(&val[0], sizeof(float), tot_ndom_pts, fd))
        {
            fprintf(stderr, "Error: unable to read file\n");
            exit(0);
        }
        for (size_t i = 0; i < val.size(); i++)
            input->domain(i, 2) = val[i];

        // rest is hard-coded for 3d

        // find extent of range
        for (size_t i = 0; i < (size_t)input->domain.rows(); i++)
        {
            if (i == 0 || input->domain(i, 2) < bounds_mins(2))
                bounds_mins(2) = input->domain(i, 2);
            if (i == 0 || input->domain(i, 2) > bounds_maxs(2))
                bounds_maxs(2) = input->domain(i, 2);
        }

        // set domain values (just equal to i, j; ie, dx, dy = 1, 1)
        int n = 0;
        for (size_t j = 0; j < (size_t)(ndom_pts(1)); j++)
            for (size_t i = 0; i < (size_t)(ndom_pts(0)); i++)
            {
                input->domain(n, 0) = i;
                input->domain(n, 1) = j;
                n++;
            }

        // extents
        bounds_mins(0) = 0.0;
        bounds_mins(1) = 0.0;
        bounds_maxs(0) = input->domain(tot_ndom_pts - 1, 0);
        bounds_maxs(1) = input->domain(tot_ndom_pts - 1, 1);
        core_mins.resize(dom_dim);
        core_maxs.resize(dom_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            core_mins(i) = bounds_mins(i);
            core_maxs(i) = bounds_maxs(i);
        }

        input->init_params();
        this->mfa = new mfa::MFA<T>(dom_dim);

        // debug
        cerr << "domain extent:\n min\n" << bounds_mins << "\nmax\n" << bounds_maxs << endl;
    }

    // read a floating point 3d scalar dataset
    // f = (x, y, z, value)
    void read_3d_scalar_data(
            const       diy::Master::ProxyWithLink& cp,
            DomainArgs& args)
    {
        DomainArgs* a = &args;
        int tot_ndom_pts = 1;
        this->geometry.min_dim = 0;
        this->geometry.max_dim = dom_dim - 1;
        int nvars = 1;
        this->vars.resize(nvars);
        this->max_errs.resize(nvars);
        this->sum_sq_errs.resize(nvars);
        this->vars[0].min_dim = dom_dim;
        this->vars[0].max_dim = this->vars[0].min_dim;
        VectorXi ndom_pts(dom_dim);
        bounds_mins.resize(pt_dim);
        bounds_maxs.resize(pt_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            ndom_pts(i)     =  a->ndom_pts[i];
            tot_ndom_pts    *= ndom_pts(i);
        }
        std::cout << "xxx tot_ndom_pts: " << tot_ndom_pts << std::endl;


        // Construct point set to contain input
        if (args.structured)
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts, ndom_pts);
        else
            input = new mfa::PointSet<T>(dom_dim, pt_dim, tot_ndom_pts);

        vector<float> val(tot_ndom_pts);

        FILE *fd = fopen(a->infile.c_str(), "rb");
        assert(fd);

        // read data values
        if (!fread(&val[0], sizeof(float), tot_ndom_pts, fd))
        {
            fprintf(stderr, "Error: unable to read file\n");
            exit(0);
        }
        for (size_t i = 0; i < val.size(); i++)
            input->domain(i, 3) = val[i];
        std::cout << "xxx val[0]: " << val[0] << std::endl;
        std::cout << "xxx val[0]: " << val.at(0) << std::endl;
        std::cout << "xxx val[1]: " << val[1] << std::endl;
        std::cout << "xxx val[1]: " << val.at(1) << std::endl;

        // rest is hard-coded for 3d

        // find extent of range
        for (size_t i = 0; i < (size_t)input->domain.rows(); i++)
        {
            if (i == 0 || input->domain(i, 3) < bounds_mins(3))
                bounds_mins(3) = input->domain(i, 3);
            if (i == 0 || input->domain(i, 3) > bounds_maxs(3))
                bounds_maxs(3) = input->domain(i, 3);
        }

        // set domain values (just equal to i, j; ie, dx, dy = 1, 1)
        int n = 0;
        for (size_t k = 0; k < (size_t)(ndom_pts(2)); k++)
            for (size_t j = 0; j < (size_t)(ndom_pts(1)); j++)
                for (size_t i = 0; i < (size_t)(ndom_pts(0)); i++)
                {
                    input->domain(n, 0) = i;
                    input->domain(n, 1) = j;
                    input->domain(n, 2) = k;
                    n++;
                }

        // extents
        bounds_mins(0) = 0.0;
        bounds_mins(1) = 0.0;
        bounds_mins(2) = 0.0;
        bounds_maxs(0) = input->domain(tot_ndom_pts - 1, 0);
        bounds_maxs(1) = input->domain(tot_ndom_pts - 1, 1);
        bounds_maxs(2) = input->domain(tot_ndom_pts - 1, 2);
        core_mins.resize(dom_dim);
        core_maxs.resize(dom_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            core_mins(i) = bounds_mins(i);
            core_maxs(i) = bounds_maxs(i);
        }

        input->init_params();
        this->mfa = new mfa::MFA<T>(dom_dim);

        // debug
        cerr << "domain extent:\n min\n" << bounds_mins << "\nmax\n" << bounds_maxs << endl;
    }
    
    void analytical_error_field(
        const diy::Master::ProxyWithLink&   cp,
        string&                             fun,                // function to evaluate
        T&                                  L1,                 // (output) L-1 norm
        T&                                  L2,                 // (output) L-2 norm
        T&                                  Linf,               // (output) L-infinity norm
        DomainArgs&                         args,               // input args
        bool                                keep_approx)        // keep the regular grid approximation we create
    {
        DomainArgs* a   = &args;
        
        // Size of grid on which to test error
        VectorXi test_pts(dom_dim);
        for (int i = 0; i < dom_dim; i++)
        {
            test_pts(i) = a->ndom_pts[i];
        }

        // Create parameters to decode at
        shared_ptr<mfa::Param<T>> grid_params = make_shared<mfa::Param<T>>(test_pts);
        
        // Create pointsets to hold decoded points and errors
        mfa::PointSet<T>* grid_approx = new mfa::PointSet<T>(grid_params, pt_dim);
        this->errs = new mfa::PointSet<T>(grid_params, pt_dim);

        // Decode on above-specified grid
        this->mfa->DecodePointSet(*(this->geometry).mfa_data, *grid_approx, 0, 0, dom_dim - 1, false);
        for (auto i = 0; i < this->vars.size(); i++)
            this->mfa->DecodePointSet(*(this->vars[i].mfa_data), *grid_approx, 0, dom_dim + i, dom_dim + i, false);

        // Copy geometric point coordinates into errs PointSet
        this->errs->domain.leftCols(dom_dim) = grid_approx->domain.leftCols(dom_dim);

        // Compute the analytical error at each point
        T sum_errs      = 0.0;                                  // sum of absolute values of errors (L-1 norm)
        T sum_sq_errs   = 0.0;                                  // sum of squares of errors (square of L-2 norm)
        T max_err       = -1.0;                                 // maximum absolute value of error (L-infinity norm)
        T true_val = 0;
        T test_val = 0;
        VectorX<T> dom_pt(dom_dim);
        for (auto pt_it = this->errs->begin(), pt_end = this->errs->end(); pt_it != pt_end; ++pt_it)
        {
            pt_it.coords(dom_pt, 0, dom_dim-1); // extract the first dom_dim coords (i.e. geometric coords)
            
            // evaluate function at dom_pt_real
            if (fun == "sinc")
                true_val = sinc(dom_pt, args, 0);      // hard-coded for one science variable
            if (fun == "sine")
                true_val = sine(dom_pt, args, 0);      // hard-codded for one science variable
            if (fun == "f16")
                true_val = f16(dom_pt);
            if (fun == "f17")
                true_val = f17(dom_pt);
            if (fun == "f18")
                true_val = f18(dom_pt);

            test_val = grid_approx->domain(pt_it.idx(), dom_dim);    // hard-coded for first science variable only

            // compute and accrue error
            T err = fabs(true_val - test_val);
            sum_errs += err;                                // L1
            sum_sq_errs += err * err;                       // L2
            if (err > max_err)                              // Linf
                max_err = err;

            this->errs->domain(pt_it.idx(), dom_dim) = err;
        }

        L1    = sum_errs;
        L2    = sqrt(sum_sq_errs);
        Linf  = max_err;

        if (keep_approx)
            this->approx = grid_approx;
        else
            delete grid_approx;
    }

    // compute error to synthethic, non-noisy function (for HEP applications)
    // outputs L1, L2, Linfinity error
    // optionally outputs true and test point locations and true and test values there
    // if optional test output wanted, caller has to allocate true_pts, test_pts, true_data, and test_data
    void analytical_error(
            const diy::Master::ProxyWithLink&     cp,
            string&                               fun,                // function to evaluate
            T&                                    L1,                 // (output) L-1 norm
            T&                                    L2,                 // (output) L-2 norm
            T&                                    Linf,               // (output) L-infinity norm
            DomainArgs&                           args,               // input args
            bool                                  output,             // whether to output test_pts, true_data, test_data or only compute error norms
            vector<vec3d>&                        true_pts,           // (output) true points locations
            float**                               true_data,          // (output) true data values (4d)
            vector<vec3d>&                        test_pts,           // (output) test points locations
            float**                               test_data)          // (output) test data values (4d)
    {
        DomainArgs* a   = &args;

        T sum_errs      = 0.0;                                  // sum of absolute values of errors (L-1 norm)
        T sum_sq_errs   = 0.0;                                  // sum of squares of errors (square of L-2 norm)
        T max_err       = -1.0;                                 // maximum absolute value of error (L-infinity norm)

        if (!this->dom_dim)
            this->dom_dim = this->mfa->dom_dim;

        size_t tot_ndom_pts = 1;
        for (auto i = 0; i < this->dom_dim; i++)
            tot_ndom_pts *= a->ndom_pts[i];

        // steps in each dimension in paramater space and real space
        vector<T> dom_step_real(this->dom_dim);                       // spacing between domain points in real space
        vector<T> dom_step_param(this->dom_dim);                      // spacing between domain points in parameter space
        for (auto i = 0; i < this->dom_dim; i++)
        {
            dom_step_param[i] = 1.0 / (double)(a->ndom_pts[i] - 1);
            dom_step_real[i] = dom_step_param[i] * (a->max[i] - a->min[i]);
        }

        // flattened loop over all the points in a domain in dimension dom_dim
        VectorXi ndom_pts(this->dom_dim);
        for (int i = 0; i < this->dom_dim; i++)
            ndom_pts(i) = a->ndom_pts[i];
        mfa::VolIterator vol_it(ndom_pts);
        fmt::print(stderr, "Testing analytical error norms over a total of {} points\n", tot_ndom_pts);
        while(!vol_it.done())
        {
            int j= vol_it.cur_iter();
            // compute current point in real and parameter space
            VectorX<T> dom_pt_real(this->dom_dim);                // one domain point in real space
            VectorX<T> dom_pt_param(this->dom_dim);               // one domain point in parameter space
            for (auto i = 0; i < this->dom_dim; i++)
            {
                dom_pt_real(i) = a->min[i] + vol_it.idx_dim(i) * dom_step_real[i];
                dom_pt_param(i) = vol_it.idx_dim(i) * dom_step_param[i];
            }

            // evaluate function at dom_pt_real
            T true_val;
            if (fun == "sinc")
                true_val = sinc(dom_pt_real, args, 0);      // hard-coded for one science variable
            if (fun == "sine")
                true_val = sine(dom_pt_real, args, 0);      // hard-codded for one science variable
            if (fun == "f16")
                true_val = f16(dom_pt_real);
            if (fun == "f17")
                true_val = f17(dom_pt_real);
            if (fun == "f18")
                true_val = f18(dom_pt_real);

            // evaluate MFA at dom_pt_param
            VectorX<T> cpt(1);                              // hard-coded for one science variable
            this->mfa->DecodePt(*(this->vars[0].mfa_data), dom_pt_param, cpt);       // hard-coded for one science variable
            T test_val = cpt(0);

            // compute and accrue error
            T err = fabs(true_val - test_val);
            sum_errs += err;                                // L1
            sum_sq_errs += err * err;                       // L2
            if (err > max_err)                              // Linf
                max_err = err;

            if (output)
            {
                vec3d true_pt, test_pt;
                true_pt.x = test_pt.x = dom_pt_real(0);
                if (this->dom_dim > 2)        // 3d or greater domain
                {
                    true_pt.y = test_pt.y = dom_pt_real(1);
                    true_pt.z = test_pt.z = dom_pt_real(2);
                }
                else if (this->dom_dim > 1)   // 2d domain
                {
                    true_pt.y = test_pt.y = dom_pt_real(1);
                    true_pt.z = true_val;
                    test_pt.z = test_val;
                }
                else                    // 1d domain
                {
                    true_pt.y = true_val;
                    test_pt.y = test_val;
                    true_pt.z = test_pt.z = 0.0;
                }

                true_pts[j] = true_pt;
                test_pts[j] = test_pt;

                true_data[0][j] = true_val;
                test_data[0][j] = test_val;

//                 fmt::print(stderr, "x={} y={} true_val={} test_val={}\n", test_pt.x, test_pt.y, true_val, test_val);
            }
            vol_it.incr_iter();
        }                                                   // for all points in flattened loop

        L1    = sum_errs;
        L2    = sqrt(sum_sq_errs);
        Linf  = max_err;
    }


    static
    void readfile(                          // add the block to the decomposition
            int gid,                        // block global id
            const Bounds<int> &core,        // block bounds without any ghost added
            const Bounds<int> &bounds,      // block bounds including any ghost region added
            const RCLink<int> &link,        // neighborhood
            diy::Master &master,            // diy master
            std::vector<int> &mapDimension, // domain dimensionality map;
            std::string &s3dfile,           // input file with data
            std::vector<unsigned> &shape,   // important, shape of the block
            int chunk,                      // vector dimension for data input (usually 2 or 3)
            int transpose,                  // diy is MPI_C_ORDER always; offer option to transpose
            DomainArgs &args)               // input args
    {
        Block<T> *b = new Block<T>;
        RCLink<int> *l = new RCLink<int>(link);
        diy::Master & m = const_cast<diy::Master&>(master);
        // write core and bounds only for first block
        if (0 == gid) {
            std::cout << "block:" << gid << "\n  core \t\t  bounds \n";
            for (int j = 0; j < 3; j++)
                std::cout << " " << core.min[j] << ":" << core.max[j] << "\t\t"
                    << " " << bounds.min[j] << ":" << bounds.max[j] << "\n";
        }
        m.add(gid, b, l);

        b->dom_dim = (int) mapDimension.size();
        diy::mpi::io::file in(master.communicator(), s3dfile, diy::mpi::io::file::rdonly);
        diy::io::BOV reader(in, shape);

        int size_data_read = 1;
        for (int j = 0; j < 3; j++)  // we know how the s3d data is organized
            size_data_read *= (bounds.max[j] - bounds.min[j] + 1);
        std::vector<float> data;
        data.resize(size_data_read * chunk);
        // read bounds will be multiplied by 3 in first direction
        Bounds<int> extBounds = bounds;
        extBounds.min[2] *= chunk; // multiply by 3
        extBounds.max[2] *= chunk; // multiply by 3
        extBounds.max[2] += chunk - 1; // the last coordinate is larger!!
        bool collective = true; //
        reader.read(extBounds, &data[0], collective);

        // assumes one scalar science variable
        b->pt_dim = b->dom_dim + 1;
        b->geometry.min_dim = 0;
        b->geometry.max_dim = b->dom_dim - 1;
        int nvars = 1;
        b->vars.resize(nvars);
        b->max_errs.resize(nvars);
        b->sum_sq_errs.resize(nvars);
        b->vars[0].min_dim = b->dom_dim;
        b->vars[0].max_dim = b->vars[0].min_dim;
        b->bounds_mins.resize(b->pt_dim);
        b->bounds_maxs.resize(b->pt_dim);
        VectorXi ndom_pts;  // this will be local now, and used in def of mfa
        ndom_pts.resize(b->dom_dim);
        int tot_ndom_pts = 1;
        for (size_t j = 0; j < b->dom_dim; j++) {
            int dir = mapDimension[j];
            int size_in_dir = -bounds.min[dir] + bounds.max[dir] + 1;
            tot_ndom_pts *= size_in_dir;
            ndom_pts(j) = size_in_dir;
            if (0 == gid)
                cerr << "  dimension " << j << " " << size_in_dir << endl;
        }

        if (!transpose && b->dom_dim > 1)
        {
            if (b->dom_dim == 2)
            {
                int tmp = ndom_pts(0);
                ndom_pts(0) = ndom_pts(1);
                ndom_pts(1) = tmp;
            }
            else if (b->dom_dim == 3)
            {
                int tmp = ndom_pts(2);
                ndom_pts(2) = ndom_pts(0);
                ndom_pts(0) = tmp; // reverse counting
            }
        }

        // Construct point set to contain input
        b->input = new mfa::PointSet<T>(b->dom_dim, b->pt_dim, tot_ndom_pts, ndom_pts);

        if (0 == gid)
            cerr << " total local size : " << tot_ndom_pts << endl;
        if (b->dom_dim == 1) // 1d problem, the dimension would be x direction
        {
            int dir0 = mapDimension[0];
            b->map_dir.push_back(dir0); // only one dimension, rest are not varying
            for (int i = 0; i < tot_ndom_pts; i++) {
                b->input->domain(i, 0) = bounds.min[dir0] + i;
                int idx = 3 * i;
                float val = 0;
                for (int k = 0; k < chunk; k++)
                    val += data[idx + k] * data[idx + k];
                val = sqrt(val);
                b->input->domain(i, 1) = val;
            }
            args.vars_nctrl_pts[0][0] = args.vars_nctrl_pts[0][dir0]; // only one direction that matters
        } else if (b->dom_dim == 2) // 2d problem, second direction would be x, first would be y
        {
            if (transpose) {
                int n = 0;
                int idx = 0;
                int dir0 = mapDimension[0]; // so now y would vary to 704 in 2d 1 block similar case for s3d (transpose)
                int dir1 = mapDimension[1];
                // we do not transpose anymore
                b->map_dir.push_back(dir0);
                b->map_dir.push_back(dir1);
                for (int i = 0; i < ndom_pts(0); i++) {
                    for (int j = 0; j < ndom_pts(1); j++) {
                        n = j * ndom_pts(0) + i;
                        b->input->domain(n, 0) = bounds.min[dir0] + i; //
                        b->input->domain(n, 1) = bounds.min[dir1] + j;
                        float val = 0;
                        for (int k = 0; k < chunk; k++)
                            val += data[idx + k] * data[idx + k];
                        val = sqrt(val);
                        b->input->domain(n, 2) = val;
                        idx += 3;
                    }
                }
            } else {
                // keep the order as Paraview, x would be the first that varies
                // corresponds to original implementation, which needs to transpose dimensions
                int n = 0;
                int idx = 0;
                int dir0 = mapDimension[1]; // so x would vary to 704 in 2d 1 block similar case
                int dir1 = mapDimension[0];
                b->map_dir.push_back(dir0);
                b->map_dir.push_back(dir1);
                for (int j = 0; j < ndom_pts(1); j++) {
                    for (int i = 0; i < ndom_pts(0); i++) {
                        b->input->domain(n, 1) = bounds.min[dir1] + j;
                        b->input->domain(n, 0) = bounds.min[dir0] + i;
                        float val = 0;
                        for (int k = 0; k < chunk; k++)
                            val += data[idx + k] * data[idx + k];
                        b->input->domain(n, 2) = sqrt(val);
                        n++;
                        idx += 3;
                    }
                }
            }
        }

        else if (b->dom_dim == 3) {
            if (transpose) {
                int n = 0;
                int idx = 0;
                b->map_dir.push_back(mapDimension[0]);
                b->map_dir.push_back(mapDimension[1]);
                b->map_dir.push_back(mapDimension[2]);
                // last dimension would correspond to x, as in the 2d example
                for (int i = 0; i < ndom_pts(0); i++)
                    for (int j = 0; j < ndom_pts(1); j++)
                        for (int k = 0; k < ndom_pts(2); k++) {
                            // max is ndom_pts(0)*ndom_pts(1)*(ndom_pts(2)-1)+ ndom_pts(0)*(ndom_pts(1)-1)+(ndom_pts(0)-1)
                            //  = ndom_pts(0)*ndom_pts(1)*ndom_pts(2) -ndom_pts(0)*ndom_pts(1) + ndom_pts(0)*ndom_pts(1)
                            //             -ndom_pts(0) + ndom_pts(0)-1 =   ndom_pts(0)*ndom_pts(1)*ndom_pts(2) - 1;
                            n = k * ndom_pts(0) * ndom_pts(1) + j * ndom_pts(0)
                                + i;
                            b->input->domain(n, 0) = bounds.min[0] + i;
                            b->input->domain(n, 1) = bounds.min[1] + j;
                            b->input->domain(n, 2) = bounds.min[2] + k;
                            float val = 0;
                            for (int k = 0; k < chunk; k++)
                                val += data[idx + k] * data[idx + k];
                            val = sqrt(val);
                            b->input->domain(n, 3) = val;
                            idx += 3;
                        }
            } else // visualization order
            {
                int n = 0;
                int idx = 0;
                b->map_dir.push_back(mapDimension[2]); // reverse
                b->map_dir.push_back(mapDimension[1]);
                b->map_dir.push_back(mapDimension[0]);
                // last dimension would correspond to x, as in the 2d example
                for (int k = 0; k < ndom_pts(2); k++)
                    for (int j = 0; j < ndom_pts(1); j++)
                        for (int i = 0; i < ndom_pts(0); i++) {
                            b->input->domain(n, 2) = bounds.min[0] + k;
                            b->input->domain(n, 1) = bounds.min[1] + j;
                            b->input->domain(n, 0) = bounds.min[2] + i; // this now corresponds to x
                            float val = 0;
                            for (int k = 0; k < chunk; k++)
                                val += data[idx + k] * data[idx + k];
                            b->input->domain(n, 3) = sqrt(val);
                            n++;
                            idx += 3;
                        }
            }
        }
        b->core_mins.resize(b->dom_dim);
        b->core_maxs.resize(b->dom_dim);
        b->overlaps.resize(b->dom_dim);
        for (int i = 0; i < b->dom_dim; i++) {
            //int index = b->dom_dim-1-i;
            int index = i;
            if (!transpose)
                index = b->dom_dim - 1 - i;
            b->bounds_mins(i) = bounds.min[mapDimension[index]];
            b->bounds_maxs(i) = bounds.max[mapDimension[index]];
            b->core_mins(i) = core.min[mapDimension[index]];
            b->core_maxs(i) = core.max[mapDimension[index]];
            // decide overlap in each direction; they should be symmetric for neighbors
            // so if block a overlaps block b, block b overlaps a the same area
            b->overlaps(i) = fabs(b->core_mins(i) - b->bounds_mins(i));
            T m2 = fabs(b->bounds_maxs(i) - b->core_maxs(i));
            if (m2 > b->overlaps(i))
                b->overlaps(i) = m2;
        }

        // set bounds_min/max for science variable (last coordinate)
        b->bounds_mins(b->dom_dim) = b->input->domain.col(b->dom_dim).minCoeff();
        b->bounds_maxs(b->dom_dim) = b->input->domain.col(b->dom_dim).maxCoeff();

        b->input->init_params();
        b->mfa = new mfa::MFA<T>(b->dom_dim);
    }

};


void max_err_cb(Block<real_t> *b,                  // local block
        const diy::ReduceProxy &rp,                // communication proxy
        const diy::RegularMergePartners &partners) // partners of the current block
{
    unsigned round = rp.round();    // current round number

    // step 1: dequeue and merge
    for (int i = 0; i < rp.in_link().size(); ++i) {
        int nbr_gid = rp.in_link().target(i).gid;
        if (nbr_gid == rp.gid()) {

            continue;
        }

        std::vector<real_t> in_vals;
        rp.dequeue(nbr_gid, in_vals);

        for (size_t j = 0; j < in_vals.size() / 2; ++j) {
            if (b->max_errs_reduce[2 * j] < in_vals[2 * j]) {
                b->max_errs_reduce[2 * j] = in_vals[2 * j];
                b->max_errs_reduce[2 * j + 1] = in_vals[2 * j + 1]; // received from this block
            }
        }
    }

    // step 2: enqueue
    for (int i = 0; i < rp.out_link().size(); ++i) // redundant since size should equal to 1
    {
        // only send to root of group, but not self
        if (rp.out_link().target(i).gid != rp.gid()) {
            rp.enqueue(rp.out_link().target(i), b->max_errs_reduce);
        } //else
        //fmt::print(stderr, "[{}:{}] Skipping sending to self\n", rp.gid(), round);
    }
}
