#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkTexture.h>
#include <vtkImageViewer.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkNamedColors.h>


#include <string>
using namespace std;

vtkNew<vtkActor> drawFigure( const string &filename,
                             double x, double y, double z, 
                             double hx, double hy){

  vtkNew<vtkPNGReader> PNGReader;
  PNGReader->SetFileName(filename.c_str());
  PNGReader->Update();

  // objeto textura
  vtkNew<vtkTexture> texture;
  texture->SetInputConnection(PNGReader->GetOutputPort());
  

  // objeto plano
  vtkNew<vtkPlaneSource> planeSource;
  planeSource->SetOrigin(0, 0, 0);
  planeSource->SetPoint1(hx,0,0);
  planeSource->SetPoint2(0,hy,0);
  planeSource->SetCenter(x, y, z);
  
  planeSource->Update();

  // mapea y asigna textura
  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(planeSource->GetOutputPort());

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->SetTexture(texture);

  return actor;

}


int main(int argc, char const *argv[])
{
  vtkNew<vtkNamedColors> colors;
  vtkNew<vtkActor> image1 = drawFigure( "../../pokemon_png/1.png", -5, 2, 4, 2, 2 ); //1-Bulbasaur
  vtkNew<vtkActor> image2 = drawFigure( "../../pokemon_png/6.png", 0, 0, 0, 7, 7 ); //6-Charizard
  vtkNew<vtkActor> image3 = drawFigure( "../../pokemon_png/25.png", 5, 2, 4, 1, 1 ); //25-Pikachu

  // The usual rendering stuff.
  vtkNew<vtkCamera> camera;
  camera->SetPosition(1, 1, 1);
  camera->SetFocalPoint(0, 0, 0);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetWindowName("DrawImage");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  renderer->AddActor(image1);
  renderer->AddActor(image2);
  renderer->AddActor(image3);
  renderer->SetActiveCamera(camera);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

  renWin->SetSize(600, 600);

  // interact with data
  renWin->Render();
  iren->Start();

  return 0;
}
