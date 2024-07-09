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

    vtkNew<vtkActor> drawCube(int width, int hight, int deep); //������ VTK ���
    vtkNew<vtkActor> drawCylinder(int height, int diameter); //������ VTK �������
    vtkNew<vtkActor> camsStand(int width, int hight, int deep);

    void drawCubeOCC(); //������ OCC ���
    void drawStand(int width, int diameter, int diameter_hole); // ������ ����� � ������� ���� ��������� 2 ��������, � 1 ��� ��������� � ������� ������ ��������.
                                                                // ��� �� ���� ������������ ��� ����������� ����������� �����
    void drawCamsStand(int width, int hight, int deep); // ��� VTK ���� - �������
    void showAxis(); //���������� ���
    void booleanDifferent(); //������ ������� ������� � ������ � ���������
    void Traning();
    void RotateDrawing();
    void ViewTrigger(bool);
    void Slicing();
    
};
