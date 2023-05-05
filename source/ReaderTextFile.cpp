#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkTexture.h>
#include <vtkPNGReader.h>
#include <vtkPlaneSource.h>

#include <sstream>
#include <string>
#include <fstream>
using namespace std;

vtkNew<vtkActor> drawFigure( const string &filename,
                             double x, double y, double z, 
                             double hx, double hy){

  vtkNew<vtkPNGReader> PNGReader;

  if (!PNGReader->CanReadFile(filename.c_str())) {
    cerr << "CanReadFile failed for " << filename.c_str() << "\n";
    exit(EXIT_FAILURE);
  }

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
  
  // Esta linea lo hace un poco mas divertido
  //planeSource->SetNormal( (rand()%100)/100.0, (rand()%100)/100.0, (rand()%100)/100.0 );
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

  string filename = "../../pokemon.csv";
  ifstream in(filename);
  int num;
  string name;
  double attack, defense, speed, height_m, hp, weight_kg_std;
  vtkNew<vtkNamedColors> colors;
  // The usual rendering stuff.
  vtkNew<vtkCamera> camera;
  camera->SetPosition(1, 1, 1);
  camera->SetFocalPoint(0, 0, 0);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetWindowName("DrawPokemon");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);


  while( in >> num >> name >> attack >> defense >> speed >> height_m >> hp >> weight_kg_std  ) {
    vtkNew<vtkActor> image = drawFigure( "../../pokemon_png/" + to_string(num)+".png", 
                                          attack, defense, speed, 
                                          height_m, hp  ); 

    renderer->AddActor(image);
  }
  
  renderer->SetActiveCamera(camera);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

  renWin->SetSize(600, 600);

  // interact with data
  renWin->Render();
  iren->Start();

  return 0;
}
