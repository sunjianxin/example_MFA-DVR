//--------------------------------------------------------------
// Volume rendering using MFA-DVR
//
// Jianxin Sun
// University of Nebraska-Lincoln
// jianxin.sun@huskers.unl.edu; sunjianxin66@gmail.com; tpeterka@anl.gov
//--------------------------------------------------------------

#include    <iostream>
#include    <stdio.h>

#include    <mfa/mfa.hpp>
#include    <diy/master.hpp>
#include    <diy/io/block.hpp>
#include    "opts.h"
#include    "block.hpp"

#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkUnstructuredGridVolumeRayCastMapper.h>
#include <vtkUnstructuredGridVolumeZSweepMapper.h>
#include <vtkProjectedTetrahedraMapper.h>
#include <vtkProjectedTetrahedraMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredPointsReader.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkTransform.h>
#include <vtkAxesActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkStructuredPoints.h>
#include <vtkThreshold.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkCaptionActor2D.h>
#include <vtkNew.h>
#include <vtkVersion.h>
#include <vtkPNGWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtksys/RegularExpression.hxx>
#include <vtksys/SystemTools.hxx>

#define SHADEON

#define SETTRANSFERFUNCTION_MARSCHNER_SIMULATED_CS()                  \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(125, 0.0);                        \
  opacityTransferFunction->AddPoint(125.1, 1.0);                      \
  opacityTransferFunction->AddPoint(255, 1.0)
#define SETCOLORMAP_MARSCHNER_SIMULATED_CS()                          \
  colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);                   \
  colorTransferFunction->AddRGBPoint(125, 1, 1, 1);                   \
  colorTransferFunction->AddRGBPoint(255.0, 1, 1, 1)

#define SETTRANSFERFUNCTION_MARSCHNER_SIMULATED()                     \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(0.49019607843137253, 0.0);        \
  opacityTransferFunction->AddPoint(0.4905882352941176, 1.0);         \
  opacityTransferFunction->AddPoint(1, 1.0)
#define SETCOLORMAP_MARSCHNER_SIMULATED()                             \
  colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);                   \
  colorTransferFunction->AddRGBPoint(0.5, 1, 1, 1);                   \
  colorTransferFunction->AddRGBPoint(1, 1, 1, 1)

#define SETTRANSFERFUNCTION_GAUSSIANBEAM_SHADE_ORG()                  \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(200, 0.0);                        \
  opacityTransferFunction->AddPoint(200.01, 1.0);                     \
  opacityTransferFunction->AddPoint(255, 1.0)
#define SETCOLORMAP_GAUSSIANBEAM_SHADE_ORG()                          \
  colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);                   \
  colorTransferFunction->AddRGBPoint(255.0, 1, 1, 1)

#define SETTRANSFERFUNCTION_GAUSSIANBEAM_NOSHADE_ORG()                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(200,  0.0);                       \
  opacityTransferFunction->AddPoint(255, 1.0)
#define SETCOLORMAP_GAUSSIANBEAM_NOSHADE_ORG()                        \
  colorTransferFunction->AddRGBPoint(0.0, 1, 1, 1);                   \
  colorTransferFunction->AddRGBPoint(200.0, 1, 1, 1);                 \
  colorTransferFunction->AddRGBPoint(255.0, 1, 0, 0)

#define SETTRANSFERFUNCTION_ANEURISM()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(0.2627450980392157, 0.0);                         \
  opacityTransferFunction->AddPoint(0.28627450980392155, 0.25);                        \
  opacityTransferFunction->AddPoint(0.30980392156862746, 0.0);                         \
  opacityTransferFunction->AddPoint(0.39215686274509803, 0.0);                        \
  opacityTransferFunction->AddPoint(0.43137254901960786, 0.6);                        \
  opacityTransferFunction->AddPoint(0.47058823529411764, 0.0);                        \
  opacityTransferFunction->AddPoint(0.6078431372549019, 0.0);                        \
  opacityTransferFunction->AddPoint(0.6666666666666666, 1.0);                        \
  opacityTransferFunction->AddPoint(1.0, 1.0)

#define SETCOLORMAP_ANEURISM()                                       \
  colorTransferFunction->AddRGBPoint(0.0, 1.0, 1.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(0.30980392156862746,  1.0, 1.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(0.39215686274509803, 0.5, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.43137254901960786, 1.0, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.47058823529411764, 0.5, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.6078431372549019, 1.0, 0.5, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.6666666666666666, 1.0, 0.5, 0.0);            \
  colorTransferFunction->AddRGBPoint(1.0, 1.0, 0.5, 0.0)

#define SETTRANSFERFUNCTION_ANEURISM_CS()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(67, 0.0);                         \
  opacityTransferFunction->AddPoint(73, 0.25);                        \
  opacityTransferFunction->AddPoint(79, 0.0);                         \
  opacityTransferFunction->AddPoint(100, 0.0);                        \
  opacityTransferFunction->AddPoint(110, 0.6);                        \
  opacityTransferFunction->AddPoint(120, 0.0);                        \
  opacityTransferFunction->AddPoint(155, 0.0);                        \
  opacityTransferFunction->AddPoint(170, 1.0);                        \
  opacityTransferFunction->AddPoint(255, 1.0)

#define SETCOLORMAP_ANEURISM_CS()                                       \
  colorTransferFunction->AddRGBPoint(0.0, 1.0, 1.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(79,  1.0, 1.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(100, 0.5, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(110, 1.0, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(120, 0.5, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(155, 1.0, 0.5, 0.0);            \
  colorTransferFunction->AddRGBPoint(170, 1.0, 0.5, 0.0);            \
  colorTransferFunction->AddRGBPoint(255, 1.0, 0.5, 0.0)

namespace {
void RestoreSceneFromFile(std::string fileName, vtkCamera* camera);
}

int main(int argc, char* argv[])
{
    // Initialize MPI
    diy::mpi::environment  env(argc, argv);             // equivalent of MPI_Init(argc, argv)/MPI_Finalize()
    diy::mpi::communicator world;                       // equivalent of MPI_COMM_WORLD

    std::string infile_vtk;         // vtk input file
    std::string infile_mfa;         // mfa input file
    int         size;               // size of vtk input file on each dimension
    std::string method;             // method used for ray traversal, dis/mfa
    std::string data;               // dataset name
    bool        help;               // show help

    // Get command line arguments
    opts::Options ops;
    ops >> opts::Option('v', "vtk",      infile_vtk,     " VTK input file name, this has to be vtkStructuredPoint format");
    ops >> opts::Option('m', "mfa",      infile_mfa,     " MFA input file name");
    ops >> opts::Option('d', "size",     size,           " Sample size of VTK input file on each dimension");
    ops >> opts::Option('e', "method",   method,         " Method used for ray traversal, dis/mfa (dis for vtk default of NNS or trilinear interpolation; mfa for MFA decoding)");
    ops >> opts::Option('n', "data",     data,           " Dataset name for respective setting of transfer functions");
    ops >> opts::Option('h', "help",     help,           " show help");

    if (!ops.parse(argc, argv) || help)
    {
        if (world.rank() == 0)
            std::cout << ops;
        return 1;
    }

    // Initialize DIY
    diy::FileStorage storage("./DIY.XXXXXX");
    diy::Master master(world,
                       1,
                       -1,
                       &Block<real_t>::create,
                       &Block<real_t>::destroy);
    diy::ContiguousAssigner assigner(world.size(), -1);   // number of blocks set by read_blocks()

    // Read a previously-solved MFA from disk into a DIY block
    std::cout << "mfa: " << infile_mfa << std::endl;
    diy::io::read_blocks(infile_mfa.c_str(), world, assigner, master, &Block<real_t>::load);
    std::cout << master.size() << " blocks read from file "<< infile_mfa << "\n\n";
    void* bblock = master.block(0);

    bool mfaDecoding;
    if (method == "mfa") {
        std::cout << "Using MFA Decoding" << std::endl;
        mfaDecoding = true;
    } else if (method == "dis") {
        std::cout << "Using VTK Default Interpolation" << std::endl;
        mfaDecoding = false;
    }

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // Create the reader for the data
    vtkNew<vtkStructuredPointsReader> reader;
    reader->SetFileName(infile_vtk.c_str());
    reader->Update();

    // Create transfer mapping scalar value to opacity
    vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
#if defined(SHADEON)
    if (data == "gaussianbeam") {
        SETTRANSFERFUNCTION_GAUSSIANBEAM_SHADE_ORG();
    }
    if (data == "vertebra") {
        //// SETTRANSFERFUNCTION_MARSCHNER_SIMULATED_CS(); // color scale
        //// SETTRANSFERFUNCTION_MARSCHNER_SIMULATED(); // non color scale
        SETTRANSFERFUNCTION_ANEURISM_CS();
        // SETTRANSFERFUNCTION_ANEURISM();
    }
#else
    if (data == "gaussianbeam") {
        SETTRANSFERFUNCTION_GAUSSIANBEAM_NOSHADE_ORG();
    }
#endif

    // Create transfer mapping scalar value to color
    vtkNew<vtkColorTransferFunction> colorTransferFunction;
#if defined(SHADEON)
    if (data == "gaussianbeam") {
        SETCOLORMAP_GAUSSIANBEAM_SHADE_ORG();
    }
    if (data == "vertebra") {
        //// SETCOLORMAP_MARSCHNER_SIMULATED_CS(); //color scale
        //// SETCOLORMAP_MARSCHNER_SIMULATED(); // non color scale
        SETCOLORMAP_ANEURISM_CS();
        // SETCOLORMAP_ANEURISM();
    }
#else
    if (data == "gaussianbeam") {
        SETCOLORMAP_GAUSSIANBEAM_NOSHADE_ORG();
    }
#endif

    // The property describes how the data will look
    vtkNew<vtkVolumeProperty> volumeProperty;
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
#if defined(SHADEON)
    volumeProperty->ShadeOn();
#endif
    // volumeProperty->SetInterpolationTypeToNearest();
    volumeProperty->SetInterpolationTypeToLinear();

    // The mapper / ray cast function know how to render the data
    vtkNew<vtkFixedPointVolumeRayCastMapper> volumeMapper;
    volumeMapper->SetInputConnection(reader->GetOutputPort());
    volumeMapper->SetMFAInputConnection(bblock, mfaDecoding, size);
    if (data == "gaussianbeam") {
        volumeMapper->SetSampleDistance(0.02);
    }
    if (data == "vertebra") {
        volumeMapper->SetSampleDistance(0.25);
    }
    std::cout << "Sample Distance: " << volumeMapper->GetSampleDistance() << std::endl;

    vtkNew<vtkVolume> volume;
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    // Create axis
    vtkNew<vtkTransform> transform;
    vtkNew<vtkAxesActor> axes;
    double l[3];
    l[0] = (volume->GetBounds()[1] - volume->GetBounds()[0]) * 1.5;
    l[1] = (volume->GetBounds()[3] - volume->GetBounds()[2]) * 1.5;
    l[2] = (volume->GetBounds()[5] - volume->GetBounds()[4]) * 1.5;
    // std::cout << volume->GetBounds()[1] - volume->GetBounds()[0]
    //           << volume->GetBounds()[3] - volume->GetBounds()[2]
    //           << volume->GetBounds()[5] - volume->GetBounds()[4] << std::endl;
    axes->SetTotalLength(l);
    axes->SetConeRadius(0.1);
    vtkSmartPointer<vtkTextProperty> tprop =
    vtkSmartPointer<vtkTextProperty>::New();
    tprop->ItalicOn();
    tprop->ShadowOn();
    tprop->SetFontFamilyToTimes();
    // std::cout << tprop->GetFontSize();
    tprop->SetFontSize(18);
    axes->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
    axes->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(30);
    axes->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
    axes->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(30);
    axes->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
    axes->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(30);
    // ren1->AddActor(axes);

    ren1->AddVolume(volume);
    ren1->SetBackground(255, 255, 255);
    // ren1->SetBackground(0, 0, 0);
    ren1->GetActiveCamera()->Elevation(-90);
    ren1->ResetCameraClippingRange();
    ren1->ResetCamera();
    ren1->GetActiveCamera()->Azimuth(45);
    ren1->ResetCameraClippingRange();
    ren1->ResetCamera();
    ren1->GetActiveCamera()->Elevation(45);
    ren1->ResetCameraClippingRange();
    ren1->ResetCamera();

    renWin->SetSize(1000, 1000);
    if (data == "gaussianbeam") {
        RestoreSceneFromFile("../../../scene/camera_gaussianbeam_directSideView_3.txt", ren1->GetActiveCamera());
    }
    if (data == "vertebra") {
        RestoreSceneFromFile("../../../scene/camera_aneurism.txt", ren1->GetActiveCamera());
    }
    //// RestoreSceneFromFile("../../../scene/camera_marschner_simulated_directSideView.txt", ren1->GetActiveCamera());
    renWin->Render();
    renWin->SetWindowName("Result");

    iren->Start();

    return EXIT_SUCCESS;
}

namespace {
#include <fstream>
void RestoreSceneFromFile(std::string fileName, vtkCamera* camera)
{
  std::ifstream saveFile(fileName);
  std::string line;

  vtksys::RegularExpression reCP("^Camera:Position");
  vtksys::RegularExpression reCFP("^Camera:FocalPoint");
  vtksys::RegularExpression reCVU("^Camera:ViewUp");
  vtksys::RegularExpression reCVA("^Camera:ViewAngle");
  vtksys::RegularExpression reCCR("^Camera:ClippingRange");
  vtksys::RegularExpression floatNumber(
      "[^0-9\\.\\-]*([0-9e\\.\\-]*[^,])[^0-9\\.\\-]*([0-9e\\.\\-]*[^,])[^0-9\\."
      "\\-]*([0-9e\\.\\-]*[^,])");
  vtksys::RegularExpression floatScalar("[^0-9\\.\\-]*([0-9\\.\\-e]*[^,])");

  while (std::getline(saveFile, line) && !saveFile.eof())
  {
    if (reCFP.find(line))
    {
      std::string rest(line, reCFP.end());
      if (floatNumber.find(rest))
      {
        camera->SetFocalPoint(atof(floatNumber.match(1).c_str()),
                              atof(floatNumber.match(2).c_str()),
                              atof(floatNumber.match(3).c_str()));
      }
    }
    else if (reCP.find(line))
    {
      std::string rest(line, reCP.end());
      if (floatNumber.find(rest))
      {
        camera->SetPosition(atof(floatNumber.match(1).c_str()),
                            atof(floatNumber.match(2).c_str()),
                            atof(floatNumber.match(3).c_str()));
      }
    }
    else if (reCVU.find(line))
    {
      std::string rest(line, reCVU.end());
      if (floatNumber.find(rest))
      {
        camera->SetViewUp(atof(floatNumber.match(1).c_str()),
                          atof(floatNumber.match(2).c_str()),
                          atof(floatNumber.match(3).c_str()));
      }
    }
    else if (reCVA.find(line))
    {
      std::string rest(line, reCVA.end());
      if (floatScalar.find(rest))
      {
        camera->SetViewAngle(atof(floatScalar.match(1).c_str()));
      }
    }
    else if (reCCR.find(line))
    {
      std::string rest(line, reCCR.end());
      if (floatNumber.find(rest))
      {
        camera->SetClippingRange(atof(floatNumber.match(1).c_str()),
                                 atof(floatNumber.match(2).c_str()));
      }
    }
    else
    {
      std::cout << "Unrecognized line: " << line << std::endl;
    }
  }
  saveFile.close();
}
}
