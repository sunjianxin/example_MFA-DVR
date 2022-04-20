//--------------------------------------------------------------
// Visualize points at a single mass value from a stored MFA
//
// Jianxin Sun
// Argonne National Laboratory
// sunj@anl.gov; tpeterka@anl.gov
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
#include <vtkLight.h>
#include <vtkLightActor.h>
#include <vtkLightCollection.h>

#include <vtksys/RegularExpression.hxx>
#include <vtksys/SystemTools.hxx>

#define SHADEON

#define SETTRANSFERFUNCTION_ANEURISM_ISO()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(0.29803921568, 0.0);                         \
  opacityTransferFunction->AddPoint(0.30196078431, 1.0);                         \
  opacityTransferFunction->AddPoint(1.0, 1.0)

#define SETCOLORMAP_ANEURISM_ISO()                                       \
  colorTransferFunction->AddRGBPoint(0.0, 1.0, 1.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(1.0,  1.0, 1.0, 1.0)

#define SETTRANSFERFUNCTION_ANEURISM_ISO_CS()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(76, 0.0);                         \
  opacityTransferFunction->AddPoint(77, 1.0);                         \
  opacityTransferFunction->AddPoint(255, 1.0)

#define SETCOLORMAP_ANEURISM_ISO_CS()                                       \
  colorTransferFunction->AddRGBPoint(0.0, 1.0, 1.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(255,  1.0, 1.0, 1.0)


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

#define SETTRANSFERFUNCTION_BONSAI()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(0.16862745098, 0.0);                         \
  opacityTransferFunction->AddPoint(0.18823529411, 0.2);                        \
  opacityTransferFunction->AddPoint(0.22745098039, 0.0);                         \
  opacityTransferFunction->AddPoint(0.27058823529, 0.0);                        \
  opacityTransferFunction->AddPoint(0.27450980392, 1.0);                        \
  opacityTransferFunction->AddPoint(1.0, 1.0)

#define SETCOLORMAP_BONSAI()                                       \
  colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.9, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.15686274509,  0.0, 0.9, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.27450980392,  0.5, 0.5, 0.5);            \
  colorTransferFunction->AddRGBPoint(1.0, 0.705882, 0.0156863, 0.14902)

#define SETTRANSFERFUNCTION_BONSAI_CS()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(43, 0.0);                         \
  opacityTransferFunction->AddPoint(48, 0.2);                        \
  opacityTransferFunction->AddPoint(58, 0.0);                         \
  opacityTransferFunction->AddPoint(69, 0.0);                        \
  opacityTransferFunction->AddPoint(70, 1.0);                        \
  opacityTransferFunction->AddPoint(255, 1.0)

#define SETCOLORMAP_BONSAI_CS()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.9, 0.0);            \
  colorTransferFunction->AddRGBPoint(40, 0.0, 0.9, 0.0);            \
  colorTransferFunction->AddRGBPoint(70,  0.5, 0.5, 0.5);            \
  colorTransferFunction->AddRGBPoint(255, 0.705882, 0.0156863, 0.14902)

#if 0
#define tf_CS()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(9, 0.0);                        \
  opacityTransferFunction->AddPoint(10, 0.5);                        \
  opacityTransferFunction->AddPoint(15, 0.0);                        \
  opacityTransferFunction->AddPoint(19, 0.0);                        \
  opacityTransferFunction->AddPoint(20, 1.0);                        \
  opacityTransferFunction->AddPoint(25, 1.0);                        \
  opacityTransferFunction->AddPoint(255, 1.0)

#define cm_CS()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 1.0, 1.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(10, 0.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(20, 1.0, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(255, 1.0, 1.0, 1.0)
#endif

#define SETTRANSFERFUNCTION_NUCLEON()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(0.11764705882352941, 0.0);                        \
  opacityTransferFunction->AddPoint(0.13725490196078433, 0.02);                        \
  opacityTransferFunction->AddPoint(0.1568627450980392, 0.0);                        \
  opacityTransferFunction->AddPoint(0.23529411764705882, 0.0);                        \
  opacityTransferFunction->AddPoint(0.2549019607843137, 0.05);                        \
  opacityTransferFunction->AddPoint(0.27450980392156865, 0.0);                        \
  opacityTransferFunction->AddPoint(0.43137254901960786, 0.0);                        \
  opacityTransferFunction->AddPoint(0.49019607843137253, 0.07);                        \
  opacityTransferFunction->AddPoint(0.5294117647058824, 0.0);                        \
  opacityTransferFunction->AddPoint(0.7647058823529411, 0.0);                        \
  opacityTransferFunction->AddPoint(0.8, 0.0);                        \
  opacityTransferFunction->AddPoint(0.803921568627451, 1.0);                        \
  opacityTransferFunction->AddPoint(1.0, 1.0)

#define SETCOLORMAP_NUCLEON()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 0.378431, 0.378431, 0.958824);            \
  colorTransferFunction->AddRGBPoint(0.1411764705882353, 0.0, 0.0, 0.660784);            \
  colorTransferFunction->AddRGBPoint(0.2549019607843137,  0.0, 0.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(0.42745098039215684,  0.0, 0.501961, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.5686274509803921,  1.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.7137254901960784,  1.0, 0.380392, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.8549019607843137,  0.419608, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(1.0, 1.0, 0.0, 0.0)



#define SETTRANSFERFUNCTION_NUCLEON_CS()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(30, 0.0);                        \
  opacityTransferFunction->AddPoint(35, 0.02);                        \
  opacityTransferFunction->AddPoint(40, 0.0);                        \
  opacityTransferFunction->AddPoint(60, 0.0);                        \
  opacityTransferFunction->AddPoint(65, 0.05);                        \
  opacityTransferFunction->AddPoint(70, 0.0);                        \
  opacityTransferFunction->AddPoint(110, 0.0);                        \
  opacityTransferFunction->AddPoint(125, 0.07);                        \
  opacityTransferFunction->AddPoint(135, 0.0);                        \
  opacityTransferFunction->AddPoint(195, 0.0);                        \
  opacityTransferFunction->AddPoint(204, 0.0);                        \
  opacityTransferFunction->AddPoint(205, 1.0);                        \
  opacityTransferFunction->AddPoint(255, 1.0)

#define SETCOLORMAP_NUCLEON_CS()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 0.378431, 0.378431, 0.958824);            \
  colorTransferFunction->AddRGBPoint(36, 0.0, 0.0, 0.660784);            \
  colorTransferFunction->AddRGBPoint(65,  0.0, 0.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(109,  0.0, 0.501961, 0.0);            \
  colorTransferFunction->AddRGBPoint(145,  1.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(182,  1.0, 0.380392, 0.0);            \
  colorTransferFunction->AddRGBPoint(218,  0.419608, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(255, 1.0, 0.0, 0.0)


#define cm_CS()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 1.0, 1.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(255, 1.0, 1.0, 1.0)



#define SETTRANSFERFUNCTION_FUEL()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(0.01568627450980392, 0.0);                         \
  opacityTransferFunction->AddPoint(0.0196078431372549, 0.1);                         \
  opacityTransferFunction->AddPoint(0.0392156862745098, 0.0);                        \
  opacityTransferFunction->AddPoint(0.19607843137254902, 0.0);                         \
  opacityTransferFunction->AddPoint(0.21568627450980393, 0.1);                        \
  opacityTransferFunction->AddPoint(0.23529411764705882, 0.0);                        \
  opacityTransferFunction->AddPoint(0.4117647058823529, 0.0);                        \
  opacityTransferFunction->AddPoint(0.43137254901960786, 0.3);                        \
  opacityTransferFunction->AddPoint(0.45098039215686275, 0.0);                        \
  opacityTransferFunction->AddPoint(0.7333333333333333, 0.0);                        \
  opacityTransferFunction->AddPoint(0.7372549019607844, 1.0);                        \
  opacityTransferFunction->AddPoint(1.0, 1.0)

#define SETCOLORMAP_FUEL()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 0.378431, 0.378431, 0.958824);            \
  colorTransferFunction->AddRGBPoint(0.1411764705882353, 0.0, 0.0, 0.660784);            \
  colorTransferFunction->AddRGBPoint(0.2549019607843137,  0.0, 0.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(0.42745098039215684,  0.0, 0.501961, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.5686274509803921,  1.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.7137254901960784,  1.0, 0.380392, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.8549019607843137,  0.419608, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(1.0, 1.0, 0.0, 0.0)


#define SETTRANSFERFUNCTION_FUEL_CS()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(4, 0.0);                         \
  opacityTransferFunction->AddPoint(5, 0.1);                         \
  opacityTransferFunction->AddPoint(10, 0.0);                        \
  opacityTransferFunction->AddPoint(50, 0.0);                         \
  opacityTransferFunction->AddPoint(55, 0.1);                        \
  opacityTransferFunction->AddPoint(60, 0.0);                        \
  opacityTransferFunction->AddPoint(105, 0.0);                        \
  opacityTransferFunction->AddPoint(110, 0.3);                        \
  opacityTransferFunction->AddPoint(115, 0.0);                        \
  opacityTransferFunction->AddPoint(187, 0.0);                        \
  opacityTransferFunction->AddPoint(188, 1.0);                        \
  opacityTransferFunction->AddPoint(255, 1.0)

#define SETCOLORMAP_FUEL_CS()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 0.378431, 0.378431, 0.958824);            \
  colorTransferFunction->AddRGBPoint(36, 0.0, 0.0, 0.660784);            \
  colorTransferFunction->AddRGBPoint(65,  0.0, 0.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(109,  0.0, 0.501961, 0.0);            \
  colorTransferFunction->AddRGBPoint(145,  1.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(182,  1.0, 0.380392, 0.0);            \
  colorTransferFunction->AddRGBPoint(218,  0.419608, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(255, 1.0, 0.0, 0.0)

#if 0
#define SETTRANSFERFUNCTION_NEGHIP_CS()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(40, 0.0);                         \
  opacityTransferFunction->AddPoint(43, 0.1);                         \
  opacityTransferFunction->AddPoint(46, 0.0);                        \
  opacityTransferFunction->AddPoint(92, 0.0);                         \
  opacityTransferFunction->AddPoint(95, 0.1);                        \
  opacityTransferFunction->AddPoint(97, 0.0);                        \
  opacityTransferFunction->AddPoint(165, 0.0);                        \
  opacityTransferFunction->AddPoint(168, 0.3);                        \
  opacityTransferFunction->AddPoint(171, 0.0);                        \
  opacityTransferFunction->AddPoint(253, 0.0);                        \
  opacityTransferFunction->AddPoint(254, 1.0);                        \
  opacityTransferFunction->AddPoint(255, 1.0)
#endif

#define SETTRANSFERFUNCTION_NEGHIP()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(0.1568627450980392, 0.0);                         \
  opacityTransferFunction->AddPoint(0.16862745098039217, 0.1);                         \
  opacityTransferFunction->AddPoint(0.1803921568627451, 0.0);                        \
  opacityTransferFunction->AddPoint(0.3607843137254902, 0.0);                         \
  opacityTransferFunction->AddPoint(0.3764705882352941, 1.0);                        \
  opacityTransferFunction->AddPoint(0.3803921568627451, 1.0);                        \
  opacityTransferFunction->AddPoint(0.6470588235294118, 1.0);                        \
  opacityTransferFunction->AddPoint(0.6588235294117647, 1.0);                        \
  opacityTransferFunction->AddPoint(0.6705882352941176, 1.0);                        \
  opacityTransferFunction->AddPoint(0.9921568627450981, 1.0);                        \
  opacityTransferFunction->AddPoint(0.996078431372549, 1.0);                        \
  opacityTransferFunction->AddPoint(1.0, 1.0)


#define SETCOLORMAP_NEGHIP()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 0.378431, 0.378431, 0.958824);            \
  colorTransferFunction->AddRGBPoint(0.1411764705882353, 0.0, 0.0, 0.660784);            \
  colorTransferFunction->AddRGBPoint(0.2823529411764706,  0.0, 0.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(0.37254901960784315,  0.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.42745098039215684,  0.0, 0.501961, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.5686274509803921,  1.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.7137254901960784,  1.0, 0.380392, 0.0);            \
  colorTransferFunction->AddRGBPoint(0.8549019607843137,  0.419608, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(1.0, 1.0, 0.0, 0.0)



#define SETTRANSFERFUNCTION_NEGHIP_CS()                                \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(40, 0.0);                         \
  opacityTransferFunction->AddPoint(43, 0.1);                         \
  opacityTransferFunction->AddPoint(46, 0.0);                        \
  opacityTransferFunction->AddPoint(92, 0.0);                         \
  opacityTransferFunction->AddPoint(96, 1.0);                        \
  opacityTransferFunction->AddPoint(97, 1.0);                        \
  opacityTransferFunction->AddPoint(165, 1.0);                        \
  opacityTransferFunction->AddPoint(168, 1.0);                        \
  opacityTransferFunction->AddPoint(171, 1.0);                        \
  opacityTransferFunction->AddPoint(253, 1.0);                        \
  opacityTransferFunction->AddPoint(254, 1.0);                        \
  opacityTransferFunction->AddPoint(255, 1.0)


#define SETCOLORMAP_NEGHIP_CS()                                       \
  /*colorTransferFunction->AddRGBPoint(0.0, 0.096274, 0.249216, 0.127647);*/            \
  colorTransferFunction->AddRGBPoint(0.0, 0.378431, 0.378431, 0.958824);            \
  colorTransferFunction->AddRGBPoint(36, 0.0, 0.0, 0.660784);            \
  colorTransferFunction->AddRGBPoint(72,  0.0, 0.0, 1.0);            \
  colorTransferFunction->AddRGBPoint(95,  0.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(109,  0.0, 0.501961, 0.0);            \
  colorTransferFunction->AddRGBPoint(145,  1.0, 1.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(182,  1.0, 0.380392, 0.0);            \
  colorTransferFunction->AddRGBPoint(218,  0.419608, 0.0, 0.0);            \
  colorTransferFunction->AddRGBPoint(255, 1.0, 0.0, 0.0)



#if 0
#define SETTRANSFERFUNCTION_TACC_TURBULENCE()                              \
  opacityTransferFunction->AddPoint(0.0,  0.0);                         \
  opacityTransferFunction->AddPoint(0.00392156862745097,  0.0);                         \
  opacityTransferFunction->AddPoint(0.00392156862745098, 1.0);                         \
  opacityTransferFunction->AddPoint(1.0, 1.0)

#define SETCOLORMAP_TACC_TURBULENCE()                                     \
  colorTransferFunction->AddRGBPoint(0.0, 0.231373, 0.298039, 0.752941);            \
  colorTransferFunction->AddRGBPoint(0.00392156862745098,  0.865, 0.865, 0.865);            \
  colorTransferFunction->AddRGBPoint(0.00784313725490196,  0.705882, 0.0156863, 0.14902);            \
  colorTransferFunction->AddRGBPoint(1.0,  0.705882, 0.0156863, 0.14902)
#endif
#if 1
#define SETTRANSFERFUNCTION_TACC_TURBULENCE()                              \
  opacityTransferFunction->AddPoint(0,  0.0);                         \
  opacityTransferFunction->AddPoint(1, 1.0);                         \
  opacityTransferFunction->AddPoint(255, 1.0)

#define SETCOLORMAP_TACC_TURBULENCE()                                     \
  colorTransferFunction->AddRGBPoint(0, 0.231373, 0.298039, 0.752941);            \
  colorTransferFunction->AddRGBPoint(1,  0.865, 0.865, 0.865);            \
  colorTransferFunction->AddRGBPoint(2,  0.705882, 0.0156863, 0.14902);            \
  colorTransferFunction->AddRGBPoint(255,  0.705882, 0.0156863, 0.14902)
#endif

namespace {

void KeypressCallbackFunction(vtkObject* caller, long unsigned int eventId,
                              void* clientData, void* callData);

}
#include <vtkActor.h>
#include <vtkCamera.h>
namespace {
void SaveSceneToFile(std::string fileName, vtkCamera* camera);
}
#include <vtkActor.h>
#include <vtkCamera.h>
namespace {
void RestoreSceneFromFile(std::string fileName, vtkCamera* camera);
}

class vtkTimerCallback2 : public vtkCommand
{
public:
  vtkTimerCallback2() = default;
  ~vtkTimerCallback2() = default;

  int timerId = 0;
  static vtkTimerCallback2* New()
  {
    vtkTimerCallback2* cb = new vtkTimerCallback2;
    cb->TimerCount = 0;
    return cb;
  }
  virtual void Execute(vtkObject* caller, unsigned long eventId,
                       void* vtkNotUsed(callData))
  {
    vtkRenderWindowInteractor* iren =
        dynamic_cast<vtkRenderWindowInteractor*>(caller);
    if (vtkCommand::TimerEvent == eventId)
    {
      ++this->TimerCount;
    }
    if (TimerCount < 30*10 + 1) // 30 frames per second for 10 seconds, image index from 1 to 300, index 0 was not saved.
    {
#if 1 
      // Screenshot
      vtkNew<vtkWindowToImageFilter> windowToImageFilter;
      windowToImageFilter->SetInput(iren->GetRenderWindow());
      windowToImageFilter->SetScale(1); // image quality
      // windowToImageFilter->SetInputBufferTypeToRGBA(); // also record the alpha
      windowToImageFilter->SetInputBufferTypeToRGB(); // also record the alpha
      windowToImageFilter->ReadFrontBufferOff();       // read from the back buffer
      // windowToImageFilter->Update();

      vtkNew<vtkPNGWriter> writer;
      std::string fileName = "screenShot/" + std::to_string(TimerCount) + ".png";
      writer->SetFileName(fileName.c_str());
      writer->SetInputConnection(windowToImageFilter->GetOutputPort());
      writer->Write();
      std::cout << "=======Saving Done!" << endl;
#endif
      // volume->RotateZ(1.2); // For structured Gaussian Beam and Marschner-lobb Demo
      // volume->RotateZ(1.2); // For Aneurysm
      // volume->RotateY(1.2); // For Bonsai
      // volume->RotateX(1.2); // For Fuel
      volume->RotateZ(1.2); // For Nucleon
      // iren->GetRenderWindow()->Render();

      // iren->GetRenderWindow()->Render();
    }
    else
    {
      iren->DestroyTimer();
    }
  }

private:
  int TimerCount = 0;

public:
  // vtkActor* actor;
  vtkVolume* volume;
};

int main(int argc, char* argv[])
{
    /*
    if (argc < 6)
    {
        std::cout << endl;
        std::cout << "Usage: " << argv[0] << " INPUT.vtk" << " INPUT.out" << " SIZE" << " DATASET" << " MODE\n" << std::endl;
        std::cout << "" << "INPUT.vtk is the input volume data in VTK StructuredPoints format." << std::endl;
        std::cout << "" << "INPUT.out is the MFA encoded input volume data." << std::endl;
        std::cout << "" << "SIZE is how many samples on each dimension." << std::endl;
        std::cout << "" << "DATASET is name of the dataset." << std::endl;
        std::cout << "" << "MODE can be \"dis\" or \"mfa\":" << std::endl;
        std::cout << "" << "    \"dis\": for Ray Traversal using Linear Interpolation" << std::endl;
        std::cout << "" << "    \"mfa\": for Ray Traversal using MFA Decoding" << std::endl;
        std::cout << endl;
        return EXIT_FAILURE;
    }
    */
    // initialize MPI
    diy::mpi::environment  env(argc, argv);             // equivalent of MPI_Init(argc, argv)/MPI_Finalize()
    diy::mpi::communicator world;                       // equivalent of MPI_COMM_WORLD

    std::string                      infile_vtk;         // vtk input file
    std::string                      infile_mfa;         // mfa input file
    int                         size;               // size of vtk input file on each dimension
    std::string                      name;               // name of the dataset
    std::string                      method;             // method used for ray traversal, dis/mfa
    bool                        help;               // show help

    // get command line arguments
    opts::Options ops;
    ops >> opts::Option('v', "vtk",      infile_vtk,     " VTK input file name, this has to be vtkStructuredPoint format");
    ops >> opts::Option('m', "mfa",      infile_mfa,     " MFA input file name");
    ops >> opts::Option('d', "size",     size,           " Sample size of VTK input file on each dimension ");
    ops >> opts::Option('n', "name",     name,           " Name of the dataset");
    ops >> opts::Option('e', "method",   method,         " Method used for ray traversal, dis/mfa (dis for vtk default; mfa for MFA decoding)");
    ops >> opts::Option('h', "help",     help,       " show help");

    if (!ops.parse(argc, argv) || help)
    {
        if (world.rank() == 0)
            std::cout << ops;
        return 1;
    }


    // initialize DIY
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
        std::cout << "Using VTK Structured Points Interpolation" << std::endl;
        mfaDecoding = false;
    }

  
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    // renWin->DebugOn();

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // Create the reader for the data
    vtkNew<vtkStructuredPointsReader> reader;
    reader->SetFileName(infile_vtk.c_str());
    reader->Update();

    double r[2];
    reader->GetOutput()->GetScalarRange(r);
    std::cout << "min: " << r[0] << std::endl;
    std::cout << "max: " << r[1] << std::endl;

    // Create transfer mapping scalar value to opacity
    vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
#if defined(SHADEON)
    // SETTRANSFERFUNCTION_ANEURISM_CS();
    // SETTRANSFERFUNCTION_ANEURISM();
    // SETTRANSFERFUNCTION_ANEURISM_ISO_CS();
    // SETTRANSFERFUNCTION_ANEURISM_ISO();

    // SETTRANSFERFUNCTION_BONSAI_CS();
    // SETTRANSFERFUNCTION_BONSAI();
    // SETTRANSFERFUNCTION_FUEL_CS();
    // SETTRANSFERFUNCTION_FUEL();
    // tf_CS();
    SETTRANSFERFUNCTION_NUCLEON_CS();
    // SETTRANSFERFUNCTION_NUCLEON();
#else
    SETTRANSFERFUNCTION_TACC_TURBULENCE();
#endif

    // Create transfer mapping scalar value to color
    vtkNew<vtkColorTransferFunction> colorTransferFunction;
#if defined(SHADEON)
    // SETCOLORMAP_ANEURISM_CS();
    // SETCOLORMAP_ANEURISM();
    // SETCOLORMAP_ANEURISM_ISO_CS();
    // SETCOLORMAP_ANEURISM_ISO();

    // SETCOLORMAP_BONSAI_CS();
    // SETCOLORMAP_BONSAI();
    // SETCOLORMAP_FUEL_CS();
    // SETCOLORMAP_FUEL();
    // cm_CS();
    SETCOLORMAP_NUCLEON_CS();
    // SETCOLORMAP_NUCLEON();
#else
    SETCOLORMAP_TACC_TURBULENCE();
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
    // vtkNew<vtkSmartVolumeMapper> volumeMapper;
    // vtkNew<vtkGPUVolumeRayCastMapper> volumeMapper;
    volumeMapper->SetInputConnection(reader->GetOutputPort());
    volumeMapper->SetMFAInputConnection(bblock, mfaDecoding, size);

    // The volume holds the mapper and the property and
    // can be used to position/orient the volume
    vtkNew<vtkVolume> volume;
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    // An interactor
    // vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    // renderWindowInteractor->SetRenderWindow(renWin);

    // vtkNew<vtkCallbackCommand> keypressCallback;
    // keypressCallback->SetCallback(KeypressCallbackFunction);
    // renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);

    // vtkNew<vtkTransform> transform;
    // transform->Translate(0.0, 0.0, 0.0);
    // vtkNew<vtkAxesActor> axes;
    // axes->SetUserTransform(transform);
    // ren1->AddActor(axes);
    // ---------------------------axis---------------------------
    // Add axes
    vtkNew<vtkTransform> transform;

    // transform->Translate(actor->GetCenter()[0], actor->GetCenter()[1], actor->GetCenter()[2]);
    // transform->Translate(0.0, 0.0, 0.0);
    // axes
    vtkNew<vtkAxesActor> axes;

    double l[3];
    l[0] = (volume->GetBounds()[1] - volume->GetBounds()[0]) * 1.5;
    l[1] = (volume->GetBounds()[3] - volume->GetBounds()[2]) * 1.5;
    l[2] = (volume->GetBounds()[5] - volume->GetBounds()[4]) * 1.5;

    std::cout << volume->GetBounds()[1] - volume->GetBounds()[0]
              << volume->GetBounds()[3] - volume->GetBounds()[2]
              << volume->GetBounds()[5] - volume->GetBounds()[4] << std::endl;

    axes->SetTotalLength(l);
    // axes->SetUserTransform(transform);
    axes->SetConeRadius(0.1);

    vtkSmartPointer<vtkTextProperty> tprop =
    vtkSmartPointer<vtkTextProperty>::New();
    tprop->ItalicOn();
    tprop->ShadowOn();
    tprop->SetFontFamilyToTimes();
    std::cout << tprop->GetFontSize();
    tprop->SetFontSize(18);
    // axes->GetXAxisCaptionActor2D()->SetCaptionTextProperty(tprop);

    axes->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
    axes->GetXAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(30);
    axes->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
    axes->GetYAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(30);
    axes->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleMode(vtkTextActor::TEXT_SCALE_MODE_NONE);
    axes->GetZAxisCaptionActor2D()->GetTextActor()->GetTextProperty()->SetFontSize(30);
    // ren1->AddActor(axes);
    // ---------------------------axis---------------------------
  
    // transform->Translate(-volume->GetCenter()[0], -volume->GetCenter()[1], 0);
    // volume->SetUserTransform(transform);
    // volume->SetPosition(-volume->GetCenter()[0], -volume->GetCenter()[1], 0);
    // volume->SetOrigin(20, 20, 0);
    // std::cout << "=======INFO: " << volume->GetCenter()[0] << ", " << volume->GetCenter()[1] << std::endl;
    // volume->SetOrigin(volume->GetCenter()[0], volume->GetCenter()[1], 0); 
   
    /* for nns */
#if 0
    // for ball 8x8x8 dataset when using nns, ball and gaussian beam dataset
    double position[3] = {-0.5, -0.5, -0.5};
    volume->SetPosition(position);
    volume->SetOrigin(volume->GetCenter()[0] + 1, volume->GetCenter()[1] + 1, 0);
#endif
#if 0
    // for Marchner_lobb 64x64x64 dataset with range [0, 7] when using nns
    double shift = 7.0/63.0;
    cerr << "sssssss" << -shift << endl;
    double position[3] = {-shift/2, -shift/2, -shift/2};
    volume->SetPosition(position);
    volume->SetOrigin(volume->GetCenter()[0] + shift, volume->GetCenter()[1] + shift, 0);
    // volume->SetOrigin(volume->GetCenter()[0], volume->GetCenter()[1], volume->GetCenter()[2]);
    cerr << "[0]" << volume->GetCenter()[0] << endl;
    cerr << "[1]" << volume->GetCenter()[1] << endl;
    cerr << "[2]" << volume->GetCenter()[2] << endl;
#endif
#if 0
    // for Marchner_lobb 64x64x64 dataset when using nns
    double position[3] = {-2, -2, -2};
    volume->SetPosition(position);
    volume->SetOrigin(volume->GetCenter()[0] + 4, volume->GetCenter()[1] + 4, 0);
#endif
#if 0
    // for Marchner_lobb 256x256x256 dataset when using nns
    double position[3] = {-0.5, -0.5, -0.5};
    volume->SetPosition(position);
    volume->SetOrigin(volume->GetCenter()[0] + 1, volume->GetCenter()[1] + 1, 0);
#endif

#if 0
    // for linear and mfa, and for MFAvalue+NNSgradient
    volume->SetOrigin(volume->GetCenter()[0], volume->GetCenter()[1], 0);
    std::cout << "=======: " << volume->GetCenter()[0] << volume->GetCenter()[1] << std::endl;
#endif

#if 0
    // for real dataset Aneurysm
    volume->SetOrigin(volume->GetCenter()[0], volume->GetCenter()[1], 0);
    std::cout << "=======: " << volume->GetCenter()[0] << volume->GetCenter()[1] << std::endl;
#endif
#if 0
    // for real dataset Aneurysm NNS
    double shift = 254.0/127.0;
    double position[3] = {-shift/2, -shift/2, -shift/2};
    volume->SetPosition(position);
    volume->SetOrigin(volume->GetCenter()[0] + shift, volume->GetCenter()[1] + shift, 0);
#endif

#if 0
    // for real dataset Bonsai
    volume->SetOrigin(volume->GetCenter()[0], 0, volume->GetCenter()[1]);
    std::cout << "=======: " << volume->GetCenter()[0] << volume->GetCenter()[1] << std::endl;
#endif

#if 0
    // for real dataset Fuel
    volume->SetOrigin(0, volume->GetCenter()[0], volume->GetCenter()[1]);
    std::cout << "=======: " << volume->GetCenter()[0] << volume->GetCenter()[1] << std::endl;
#endif
#if 0
    // for real dataset Fuel NNS
    double shift = 255.0/63.0;
    double position[3] = {-shift/2, -shift/2, -shift/2};
    volume->SetPosition(position);
    volume->SetOrigin(0, volume->GetCenter()[0] + shift, volume->GetCenter()[1] + shift);
#endif


#if 1
    // for real dataset Nucleon
    volume->SetOrigin(volume->GetCenter()[0], volume->GetCenter()[1], 0);
    std::cout << "=======: " << volume->GetCenter()[0] << volume->GetCenter()[1] << std::endl;
#endif




    ren1->AddVolume(volume);
    ren1->SetBackground(255, 255, 255);
    // ren1->SetBackground(0, 0, 0);
    // SETCAMERA_BONSAI();
    // ren1->GetActiveCamera()->Azimuth(45);
    ren1->GetActiveCamera()->Elevation(-90);
    //// ren1->GetActiveCamera()->Azimuth(15);
    // ren1->GetActiveCamera()->Elevation(30);
    // ren1->GetActiveCamera()->Pitch(45); // 
    // ren1->GetActiveCamera()->Elevation(90); //
    ren1->ResetCameraClippingRange();
    ren1->ResetCamera();
    ren1->GetActiveCamera()->Azimuth(45);
    // ren1->GetActiveCamera()->Pitch(-45);
    ren1->ResetCameraClippingRange();
    ren1->ResetCamera();
    ren1->GetActiveCamera()->Elevation(45);
    ren1->ResetCameraClippingRange();
    ren1->ResetCamera();

    double lightPosition[3] = {3.5, 3.5, 10};
    // Create a light
    double lightFocalPoint[3] = {3.5, 3.5, 3.5};
    vtkNew<vtkLight> light;
    light->SetLightTypeToSceneLight();
    light->SetPosition(lightPosition[0], lightPosition[1], lightPosition[2]);
    light->SetPositional(true); // required for vtkLightActor below
    light->SetConeAngle(10);
    light->SetFocalPoint(lightFocalPoint[0], lightFocalPoint[1],
                           lightFocalPoint[2]);
    // light->SetDiffuseColor(colors->GetColor3d("Blue").GetData());
    // light->SetAmbientColor(colors->GetColor3d("Green").GetData());
    light->SetSpecularColor(colors->GetColor3d("White").GetData());
    vtkLightCollection* originalLights = ren1->GetLights();
    std::cout << "Originally there are " << originalLights->GetNumberOfItems() << " lights." << std::endl;

    // SaveSceneToFile("camera_test.txt", ren1->GetActiveCamera()); ////
    // renWin->SetSize(10, 10);
    // renWin->SetSize(500, 500);
    renWin->SetSize(1000, 1000);
    //// RestoreSceneFromFile("camera_aneurism.txt", ren1->GetActiveCamera());
    //// RestoreSceneFromFile("camera_bonsai.txt", ren1->GetActiveCamera());
    //// RestoreSceneFromFile("camera_fuel.txt", ren1->GetActiveCamera());
    RestoreSceneFromFile("camera_nucleon.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_marschner_3.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_marschner_simulated_2.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_marschner_simulated_64x64x64.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_marschner_simulated_256x256x256.txt", ren1->GetActiveCamera());
    //// RestoreSceneFromFile("camera_marschner_simulated_directSideView.txt", ren1->GetActiveCamera());
    //// RestoreSceneFromFile("test.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_test.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_temp.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_ball.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_gaussianbeam.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_gaussianbeam_upView.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_gaussianbeam_sideView.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_gaussianbeam_directSideView.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_gaussianbeam_directSideView_2.txt", ren1->GetActiveCamera()); // used for benchmark, shift y a little bit to prevent bunyk artifact
    //// RestoreSceneFromFile("camera_gaussianbeam_directSideView_3.txt", ren1->GetActiveCamera());
    // RestoreSceneFromFile("camera_ball_zoomIn.txt", ren1->GetActiveCamera());
    renWin->Render();
    // ren1->AddLight(light);
    // renWin->SetWindowName("Volume Rendering using MFA");
    // renWin->SetWindowName("NNS");
    // renWin->SetWindowName("Trilinear Interpolation");
    renWin->SetWindowName("Result");
    // renWin->SetWindowName("MFA-DVR");
    // ren1->GetActiveCamera();

#if 1 // starts animation
    // Initialize must be called prior to creating timer events.
    iren->Initialize();
    // Sign up to receive TimerEvent
    vtkNew<vtkTimerCallback2> cb;
    cb->volume = volume;
    iren->AddObserver(vtkCommand::TimerEvent, cb);
    int timerId = iren->CreateRepeatingTimer(2000);
    cb->timerId = timerId;
#endif
    
    iren->Start();

    std::cout << "lalala" << std::endl;
    double vector[3];
    ren1->GetActiveCamera()->GetFocalPoint(vector);
    std::cout << vector[0] << ", " << vector[1] << ", " << vector[2] << std::endl;
    SaveSceneToFile("camera.txt", ren1->GetActiveCamera());
    renWin->Render();
    iren->Start();
    
    return EXIT_SUCCESS;
}

namespace {

void KeypressCallbackFunction(vtkObject* caller,
                              long unsigned int vtkNotUsed(eventId),
                              void* vtkNotUsed(clientData),
                              void* vtkNotUsed(callData))
{
  std::cout << "Keypress callback" << std::endl;

  vtkRenderWindowInteractor* iren =
      static_cast<vtkRenderWindowInteractor*>(caller);

  std::cout << "Pressed: " << iren->GetKeySym() << std::endl;
}

} // namespace
namespace {
#include <fstream>
void SaveSceneToFile(std::string fileName, vtkCamera* camera)
{
  // Actor
  //  Position, orientation, origin, scale, usrmatrix, usertransform
  // Camera
  //  FocalPoint, Position, ViewUp, ViewAngle, ClippingRange
  std::ofstream saveFile(fileName, std::ofstream::out);
  double vector[3];
  double scalar;

  camera->GetFocalPoint(vector);
  saveFile << "Camera:FocalPoint " << vector[0] << ", " << vector[1] << ", "
           << vector[2] << std::endl;
  camera->GetPosition(vector);
  saveFile << "Camera:Position " << vector[0] << ", " << vector[1] << ", "
           << vector[2] << std::endl;
  camera->GetViewUp(vector);
  saveFile << "Camera:ViewUp " << vector[0] << ", " << vector[1] << ", "
           << vector[2] << std::endl;
  scalar = camera->GetViewAngle();
  saveFile << "Camera:ViewAngle " << scalar << std::endl;
  camera->GetClippingRange(vector);
  saveFile << "Camera:ClippingRange " << vector[0] << ", " << vector[1]
           << std::endl;
  saveFile.close();
}
} // namespace
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
} // namespace
