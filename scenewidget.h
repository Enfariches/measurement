#pragma once

class SceneWidget : public QVTKOpenGLNativeWidget {
    Q_OBJECT

public:
    explicit SceneWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    virtual ~SceneWidget() = default;
    void camera();
    void renderScene();

public slots:
    void requestRender();
    void animation();

private:
    void prepareToRender(vtkObject* caller, ulong eventId, void* callData);
    double color[3] = { 0,0.51,0.85 };

    QTimer* _renderTimer;
    vtkSmartPointer<vtkRenderer> _renderer;

    vtkNew<vtkActor> drawCube(int width, int hight, int deep); //Рисует VTK куб
    vtkNew<vtkActor> drawCylinder(int height, int diameter); //Рисует VTK цилиндр
    vtkNew<vtkActor> camsStand(int width, int hight, int deep);

    void drawCubeOCC(); //Рисует OCC куб
    void drawStand(int width, int diameter, int diameter_hole); // Рисует стэнд с помощью трех цилиндров 2 основных, и 1 для отверстия с помощью булеан операции.
                                                                // Так же есть нормализация для корректного отображения теней
    void drawCamsStand(int width, int hight, int deep); // Три VTK куба - кулачки
    void showAxis(); //Показывает ОСИ
    void booleanDifferent(); //Пример буллеан разницы с диском и цилиндром
    void Traning();
    void RotateDrawing();
    void ViewTrigger(bool);
    void Slicing();
    
};
