
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QTimer>

#include <QVTKOpenGLNativeWidget.h>

#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkMapper.h>
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkPolyDataNormals.h>
#include <vtkTransform.h>

//Готовые примитивы
#include <vtkCylinderSource.h>
#include <vtkCubeSource.h>
#include <vtkDiskSource.h>
#include <vtkProperty.h>

//OCC библиотеки
#include <IVtkTools_ShapeDataSource.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
//Ошибка
#include <BRepFilletAPI_MakeFillet.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx> 
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx> 
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <BRep_Tool.hxx>
#include <Standard_Type.hxx>
#include <Geom_Plane.hxx>

//Rotate-drawing for OCC
#include <BRepPrimAPI_MakeRevol.hxx>


//Boolean-operation for OCC
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>