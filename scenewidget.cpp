#include "pch.h"

#include "scenewidget.h"

SceneWidget::SceneWidget(QWidget *parent, Qt::WindowFlags flags) : QVTKOpenGLNativeWidget(parent) {

    _renderer = vtkSmartPointer<vtkRenderer>::New();
    _renderer->SetBackground(0.95, 0.95, 0.95);
    _renderer->AddObserver(vtkCommand::StartEvent, this, &SceneWidget::prepareToRender);

    camera();

    vtkRenderWindow *window = renderWindow();
    window->AddRenderer(_renderer);
    window->SetPointSmoothing(true);
    window->SetLineSmoothing(true);
    window->SetPolygonSmoothing(true);

    

    window->AddRenderer(_renderer);

    //Рисования
    drawStand(8,40,20); //8 40 20 //width, diameter, diameter_hole
    drawCamsStand(10, 10, 10);

    //drawCubeOCC();
    //RotateDrawing();
    //booleanDifferent();

    showAxis();
    Slicing();

    renderScene();

    ViewTrigger(false);
    _renderer->ResetCamera();
    _renderer->GetActiveCamera()->Zoom(1.5);
}

void SceneWidget::camera() {
    vtkCamera *camera = _renderer->GetActiveCamera();
    camera->SetPosition(-2, 0, 1);
}

void SceneWidget::requestRender() {
    _renderTimer->start(20);
}

void SceneWidget::renderScene() {
    _renderTimer = new QTimer(this);
    connect(_renderTimer, &QTimer::timeout, this, &SceneWidget::animation);
    _renderTimer->start(20);
}

void SceneWidget::prepareToRender(vtkObject *caller, ulong eventId, void *callData) {
    // empty
}

vtkNew<vtkActor> SceneWidget::drawCube(int width, int hight, int deep) {

    vtkNew<vtkCubeSource> Cube;
    Cube->SetCenter(0, 0, 0);
    Cube->SetXLength(width);
    Cube->SetYLength(hight);
    Cube->SetZLength(deep);
    Cube->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(Cube->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);

    _renderer->AddActor(actor);
    return actor;
}

void SceneWidget::drawCubeOCC() {
    BRepPrimAPI_MakeBox mkBox(20,20,20);
    const TopoDS_Shape& shape = mkBox.Shape();

    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<IVtkTools_ShapeDataSource> occSource;
    occSource->SetShape(new IVtkOCC_Shape(shape));
    mapper->SetInputConnection(occSource->GetOutputPort());

    vtkNew<vtkTransform> trans;
    trans->PostMultiply();
    trans->Translate(-10, -10, -10);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->SetUserTransform(trans);

    _renderer->AddActor(actor);
}

vtkNew<vtkActor> SceneWidget::drawCylinder(int height, int diameter) {
    int radius = diameter / 2;

    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetHeight(height);
    cylinder->SetRadius(radius);
    cylinder->SetResolution(30);
    cylinder->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cylinder->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);
    actor->AddPosition(0, 0, -20);
    actor->SetOrientation(90, 0, 0);

    _renderer->AddActor(actor);
    return actor;

}

void SceneWidget::drawStand(int width, int diameter, int diameter_hole) {
    /*
    * Аргументы:
    * Ширина патрона, диаметр патрона, диаметр отверстия в патроне
    */
    double color_stand[3] = { 0.83,0.83,0.83 };
    int radius = diameter / 2; //Радиус главного цилиндра
    int radiusHole = diameter_hole / 2; //Радиус отверстия

    vtkNew<vtkCylinderSource> cylinderFirst;
    cylinderFirst->SetHeight(width-2);
    cylinderFirst->SetRadius(radius+5);
    cylinderFirst->SetResolution(30);
    cylinderFirst->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cylinderFirst->GetOutput());

    vtkNew<vtkActor> actor; //Стойка
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);
    actor->AddPosition(0, 0, -50);
    actor->SetOrientation(90, 0, 0); //z,x,y
    actor->GetProperty()->SetColor(color_stand);

    _renderer->AddActor(actor);

    vtkNew<vtkCylinderSource> cylinderSecond; //Патрон
    cylinderSecond->SetHeight(width);
    cylinderSecond->SetRadius(radius);
    cylinderSecond->SetResolution(50);
    cylinderSecond->Update();

    vtkNew<vtkCylinderSource> cylinderHole;
    cylinderHole->SetHeight(width+1);
    cylinderHole->SetRadius(radiusHole);
    cylinderHole->SetResolution(50);
    cylinderHole->Update();

    vtkNew<vtkTriangleFilter> tri0;
    tri0->SetInputData(cylinderSecond->GetOutput());
    tri0->Update();

    vtkNew<vtkTriangleFilter> tri1;
    tri1->SetInputData(cylinderHole->GetOutput());
    tri1->Update();

    vtkNew<vtkBooleanOperationPolyDataFilter> boolean_operation;
    boolean_operation->SetInputData(0, tri0->GetOutput());
    boolean_operation->SetInputData(1, tri1->GetOutput());
    boolean_operation->SetOperationToDifference();
    boolean_operation->Update();

    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputConnection(boolean_operation->GetOutputPort(0));

    vtkNew<vtkPolyDataMapper> mapperBoolean;
    mapperBoolean->SetInputConnection(normals->GetOutputPort());
    mapperBoolean->ScalarVisibilityOff();

    vtkNew<vtkActor> actorBoolean; //Актер boolean diference
    actorBoolean->SetMapper(mapperBoolean);
    actorBoolean->AddPosition(0, 0, (-52 + width));
    actorBoolean->SetOrientation(90, 0, 0); //z,x,y
    actorBoolean->GetProperty()->SetColor(color_stand);

    _renderer->AddActor(actorBoolean);
}

void SceneWidget::showAxis() {
    vtkNew<vtkAxesActor> axes;
    axes->SetTotalLength(20,20,20);
    _renderer->AddActor(axes);
}

void SceneWidget::booleanDifferent() {

    vtkNew<vtkCylinderSource> cylinderSecond; //Патрон
    cylinderSecond->SetHeight(8);
    cylinderSecond->SetRadius(20);
    cylinderSecond->SetResolution(50);
    cylinderSecond->Update();

    vtkNew<vtkCylinderSource> cylinderHole;
    cylinderHole->SetHeight(8 + 1);
    cylinderHole->SetRadius(5);
    cylinderHole->SetResolution(50);
    cylinderHole->Update();

    vtkNew<vtkTriangleFilter> tri0;
    tri0->SetInputData(cylinderSecond->GetOutput());
    tri0->Update();

    vtkNew<vtkTriangleFilter> tri1;
    tri1->SetInputData(cylinderHole->GetOutput());
    tri1->Update();

    vtkNew<vtkBooleanOperationPolyDataFilter> boolean_operation;
    boolean_operation->SetInputData(0, tri0->GetOutput());
    boolean_operation->SetInputData(1, tri1->GetOutput());
    boolean_operation->SetOperationToDifference();
    boolean_operation->Update();

    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputConnection(boolean_operation->GetOutputPort(0));

    vtkNew<vtkPolyDataMapper> mapperBoolean;
    mapperBoolean->SetInputConnection(normals->GetOutputPort());
    mapperBoolean->ScalarVisibilityOff();

    vtkNew<vtkActor> actorBoolean; //Актер boolean diference
    actorBoolean->SetMapper(mapperBoolean);
    actorBoolean->GetProperty()->SetColor(color);

    _renderer->AddActor(actorBoolean);


}

void SceneWidget::drawCamsStand(int width, int hight, int deep) {
    double color_cams[3] = { 0.5,0.5,0.5 };

    vtkNew<vtkActor> firstCam = drawCube(width,hight,deep);
    firstCam->AddPosition(0, 15, -35);
    firstCam->GetProperty()->SetColor(color_cams);

    vtkNew<vtkActor> secondCam = drawCube(width, hight, deep);
    secondCam->SetOrientation(0, 0, -120);
    secondCam->AddPosition(12, -8, -35);
    secondCam->GetProperty()->SetColor(color_cams);

    vtkNew<vtkActor> thirdCam = drawCube(width, hight, deep);
    thirdCam->SetOrientation(0, 0, 120);
    thirdCam->AddPosition(-12, -8, -35);
    thirdCam->GetProperty()->SetColor(color_cams);
}

vtkNew<vtkActor> SceneWidget::camsStand(int width, int hight, int deep) {

    vtkNew<vtkCubeSource> firstPart;
    firstPart->SetXLength(width);
    firstPart->SetYLength(hight);
    firstPart->SetZLength(deep);
    firstPart->Update();

    vtkNew<vtkCubeSource> secondPart;
    secondPart->SetCenter(0, hight * 0.5, deep * 0.5);
    secondPart->SetXLength(width + 1);
    secondPart->SetYLength(hight);
    secondPart->SetZLength(deep);
    secondPart->Update();

    vtkNew<vtkTriangleFilter> tri0;
    tri0->SetInputData(firstPart->GetOutput());
    tri0->Update();

    vtkNew<vtkTriangleFilter> tri1;
    tri1->SetInputData(secondPart->GetOutput());
    tri1->Update();

    vtkNew<vtkBooleanOperationPolyDataFilter> boolean_operation;
    boolean_operation->SetInputData(0, tri0->GetOutput());
    boolean_operation->SetInputData(1, tri1->GetOutput());
    boolean_operation->SetOperationToDifference();
    boolean_operation->Update();

    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputConnection(boolean_operation->GetOutputPort(0));

    vtkNew<vtkPolyDataMapper> mapperBoolean;
    mapperBoolean->SetInputConnection(normals->GetOutputPort());
    mapperBoolean->ScalarVisibilityOff();

    vtkNew<vtkActor> actorBoolean; //Актер boolean diference
    actorBoolean->SetMapper(mapperBoolean);
    actorBoolean->GetProperty()->SetColor(0.50,0.50,0.50);

    _renderer->AddActor(actorBoolean);
    return actorBoolean;
}

void SceneWidget::Traning() {
    //Тренировка
    gp_Pnt aPnt1(20, 0, 0);
    gp_Pnt aPnt2(20, 0, 5);
    gp_Pnt aPnt3(0, 0, 10);
    gp_Pnt aPnt4(-20, 0, 5);
    gp_Pnt aPnt5(-20, 0, 0);

    Handle(Geom_TrimmedCurve) aArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
    Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
    Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

    TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
    TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aArcOfCircle);
    TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);

    TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(aEdge1, aEdge2, aEdge3);

    gp_Ax1 xAxis = gp::OX(); //Ось Х по ней делаем отражение

    gp_Trsf aTrsf;
    aTrsf.SetMirror(xAxis);

    BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
    const TopoDS_Shape& aWire2 = aBRepTrsf.Shape();

    TopoDS_Wire aMirroredWire = TopoDS::Wire(aWire2);

    BRepBuilderAPI_MakeWire Wire;
    Wire.Add(aWire);
    Wire.Add(aMirroredWire);
    TopoDS_Wire myWireProfile = Wire.Wire();

    TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
    gp_Vec aPrismVec(0, 50, 0);
    TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

    TopExp_Explorer aEdgeExplorer(myBody, TopAbs_EDGE);
    while (aEdgeExplorer.More()) {
        TopoDS_Edge aEdge = TopoDS::Edge(aEdgeExplorer.Current());

        aEdgeExplorer.Next();
    }

    gp_Pnt neckLocation(0, 50, 0);
    gp_Dir neckNormal = gp::DY();
    gp_Ax2 neckAx2(neckLocation, neckNormal);
    TopoDS_Shape myNeck = BRepPrimAPI_MakeCylinder(neckAx2, 5, 5);
    myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

    TopoDS_Face faceToRemove;
    Standard_Real yMax = -1;
    Standard_Real a = -20 / 50;
    Standard_Real b = 1.e-3;

    for (TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE);
        aFaceExplorer.More(); aFaceExplorer.Next()) {
        TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
        Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);

        if (aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)) {
            Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);

            gp_Pnt aPnt = aPlane->Location();
            Standard_Real aY = aPnt.Y();
            if (aY > yMax) {
                yMax = aY;
                faceToRemove = aFace;
            }

        }
    }
    TopTools_ListOfShape facesToRemove;
    facesToRemove.Append(faceToRemove);
    auto newBody = BRepOffsetAPI_MakeThickSolid();
    newBody.MakeThickSolidByJoin(myBody,facesToRemove, -20 / 50, 1.e-3);


                    
    vtkNew<vtkPolyDataMapper> mapper;                          
    vtkNew<IVtkTools_ShapeDataSource> occSource;               
    occSource->SetShape(new IVtkOCC_Shape(newBody));
    mapper->SetInputConnection(occSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);
    actor->GetProperty()->EdgeVisibilityOn();

    _renderer->AddActor(actor);

}

void SceneWidget::RotateDrawing() {

    gp_Pnt aPnt1(0, 0, -10);
    gp_Pnt aPnt2(0, 0, 10);
    gp_Pnt aPnt3(0, -5, 15);
    gp_Pnt aPnt4(0, -10, 30);
    gp_Pnt aPnt5(0, -10, 40);

    Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
    Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
    Handle(Geom_TrimmedCurve) aSegment3 = GC_MakeSegment(aPnt4, aPnt5);

    TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
    TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aSegment2);
    TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(aSegment3);

    TopoDS_Wire F = BRepBuilderAPI_MakeWire(aEdge1, aEdge2, aEdge3);

    gp_Ax1 axis(gp_Pnt(5, 0, 0), gp_Dir(0, 0, 1));
    Standard_Real ang = M_PI;
    TopoDS_Shape myBody = BRepPrimAPI_MakeRevol(F, axis); // Full revol

    
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<IVtkTools_ShapeDataSource> occSource;
    occSource->SetShape(new IVtkOCC_Shape(F));
    mapper->SetInputConnection(occSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);
    //actor->GetProperty()->EdgeVisibilityOn();

    _renderer->AddActor(actor);
}

void SceneWidget::ViewTrigger(bool flag) {
    vtkCamera* camera = _renderer->GetActiveCamera();

    if (flag){
        camera->ParallelProjectionOn();
        //camera->SetParallelScale(10);
        //camera->SetClippingRange(-1000, 1000);
        camera->SetViewUp(-1, 0, 1);
        camera->SetPosition(0, 3, 0);
        camera->SetRoll(90);
    }
}

void SceneWidget::Slicing() {

    TopoDS_Shape box = BRepPrimAPI_MakeBox(5, 5, 5);
    TopoDS_Shape cylinder = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3, 7);

    auto myBody = BRepAlgoAPI_Cut(box, cylinder);

    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<IVtkTools_ShapeDataSource> occSource;
    occSource->SetShape(new IVtkOCC_Shape(myBody));
    mapper->SetInputConnection(occSource->GetOutputPort());

    vtkNew<vtkTransform> trans;
    trans->PostMultiply();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(color);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->SetUserTransform(trans);

    _renderer->AddActor(actor);
  

}

void SceneWidget::animation() {
    auto actor = _renderer->GetActors()->GetLastActor();
    vtkNew<vtkTransform> trans;
    trans->PostMultiply();

    actor->SetUserTransform(trans);

    trans->Translate(-0, -10, -10);

}