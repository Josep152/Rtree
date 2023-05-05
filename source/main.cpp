#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>

#include <vtkNamedColors.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "rtree.h"

using namespace std;

#define ndim 3

  // The ordering of the corner points on each face.
  std::array<std::array<vtkIdType, 4>, 6> ordering = {{{{0, 3, 2, 1}},
                                                       {{4, 5, 6, 7}},
                                                       {{0, 1, 5, 4}},
                                                       {{1, 2, 6, 5}},
                                                       {{2, 3, 7, 6}},
                                                       {{3, 0, 4, 7}}}};

vtkNew<vtkActor> drawCube( double x, double y, double z, 
                            double hx, double hy, double hz){
	std::array<std::array<double, 3>, 8> pts;
	pts[0] = {x,y,z};       //0,0,0
	pts[1] = {hx, y, z };  //1,0,0
	pts[2] = {hx, hy, z}; //1,1,0
	pts[3] = {x, hy, z};   //0,1,0
	pts[4] = {x, y, hz};   //0,0,1
	pts[5] = {hx,y,hz};   //1,0,1
	pts[6] = {hx,hy,hz}; //1,1,1
	pts[7] = {x,hy,hz};   //0,1,1

	vtkNew<vtkPolyData> cube;
	vtkNew<vtkPoints> points;
	vtkNew<vtkCellArray> polys;
	vtkNew<vtkFloatArray> scalars;

	for (auto i = 0ul; i < pts.size(); ++i) {
		points->InsertPoint(i, pts[i].data());
		scalars->InsertTuple1(i, i);
	}
	for (auto&& i : ordering) {
		polys->InsertNextCell(vtkIdType(i.size()), i.data());
	}

	// We now assign the pieces to the vtkPolyData.
	cube->SetPoints(points);
	cube->SetPolys(polys);
	cube->GetPointData()->SetScalars(scalars);

	// Now we'll look at it.
	vtkNew<vtkPolyDataMapper> cubeMapper;
	cubeMapper->SetInputData(cube);
	cubeMapper->SetScalarRange(cube->GetScalarRange());
	vtkNew<vtkActor> cubeActor;
	cubeActor->SetMapper(cubeMapper);
	cubeActor->GetProperty()->SetRepresentationToWireframe();

	return cubeActor;
}

vtkNew<vtkActor> drawPoint(double x, double y, double z) {
	vtkNew<vtkNamedColors> namedColors;

	vtkNew<vtkPoints> points;
	points->InsertNextPoint(x, y, z);

	vtkNew<vtkPolyData> pointsPolydata;
	pointsPolydata->SetPoints(points);


	vtkNew<vtkPolyData> polyData;

  polyData->SetPoints(points);

  vtkNew<vtkVertexGlyphFilter> glyphFilter;
  glyphFilter->SetInputData(polyData);
  glyphFilter->Update();
	

  
	// Visualization
	vtkNew<vtkPolyDataMapper> mapper;
	mapper->SetInputConnection(glyphFilter->GetOutputPort());

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);
	actor->GetProperty()->SetPointSize(10);
	actor->GetProperty()->SetColor(namedColors->GetColor3d("Olive").GetData());

	return actor;
}

int main(int argc, char* argv[]) {

  std::cout << "Hola VTK" << std::endl;
  std::cout << "Hi VTK" << std::endl;
  std::cout << "Halo VTK " << std::endl;
  double x, y, z, hx, hy, hz;

  string filename = "../../pokemon.csv";
  ifstream in(filename);
  int num;
  string name;
  double attack, defense, speed, height_m, hp, weight_kg_std;

  RTree<Pokemon,ndim> my_rtree;
  int i = 0;
  //Leemos los datos y los insertamos en el arbol Rtree
  while( i < 6 && ( in >> num >> name >> attack >> defense >> speed >> height_m >> hp >> weight_kg_std  )) {
    cout << num << ' ' << name << ' ' << attack << ' ' << defense << ' ' << speed << ' ' << height_m << ' ' << hp << ' ' << weight_kg_std << endl;
    Pokemon my_poke(name, height_m, hp, weight_kg_std);
    Point<ndim> my_point;
    my_point.coords[0] = attack; 
    my_point.coords[1] = defense; 
    my_point.coords[2] = speed;
    Record<Pokemon,ndim> my_record(my_poke, my_point); 
    my_rtree.insert(my_record);
    i++;
  }
  //Recorremos el arbol para leer los MBBs y guardarlos en un .txt llamado Cubes
  fstream newfile1;
  newfile1.open("cubes.txt",ios::out);  // creamos un txt donde se almacenaran los cubos
  my_rtree.write_Cubes(newfile1);
  newfile1.close();

  //Leemos Cubes.txt para poder dibujarlos
  string nombreArchivo = "cubes.txt";
  string line2;
  ifstream archivo(nombreArchivo.c_str());
  vtkNew<vtkNamedColors> colors;

  // The usual rendering stuff.
  vtkNew<vtkCamera> camera;
  camera->SetPosition(1, 1, 1);
  camera->SetFocalPoint(0, 0, 0);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  renWin->SetWindowName("Cubes");

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  while (getline(archivo, line2)) {
      stringstream linestream2;
      linestream2 << line2;
      linestream2 >> x >> y >> z >> hx >> hy >> hz;
      cout << x << ' ' << y << ' ' << z << ' ' << hx << ' ' << hy << ' ' << hz << endl;
      renderer->AddActor(drawCube(x, y, z, hx, hy, hz));
  }

  renderer->AddActor(drawPoint(0.5,0.5,0.5));
  renderer->SetActiveCamera(camera);
  renderer->ResetCamera();
  renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

  renWin->SetSize(600, 600);

  // interact with data
  renWin->Render();
  iren->Start();

  return EXIT_SUCCESS;
}