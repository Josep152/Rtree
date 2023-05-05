#ifndef RTREE_H
#define RTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>

using namespace std;

#define maxRecords 5

struct Pokemon {
    string name;
    double height_m;
    double hp;
    double weight_kg_std;
    Pokemon() {
        name = "";
        height_m = 0;
        hp = 0;
        weight_kg_std = 0;
    }
    Pokemon(string _name, double _height_m, double _hp, double _weight_kg_srd) {
        name = _name;
        height_m = _height_m;
        hp = _hp;
        weight_kg_std = _weight_kg_srd;
    }
    /*void operator= (Pokemon p) {
        name = p.name;
        height_m = p.height_m;
        hp = p.hp;
        weight_kg_std = p.weight_kg_std;
    }*/
};

template <int ndim>
struct Point {
    vector<double> coords;
    Point() {
        coords.resize(ndim);
    }
    void operator =(Point p) {
        coords = p.coords;
    }
    void printP () {
        for (int i = 0; i < coords.size(); i++ ) {
            cout << coords[i] << ' ';
        }
        cout << endl;
    }
    double disEu (Point a) {
        double rpta = 0;
        for (int i = 0; i < coords.size(); i++) {
            rpta += pow(coords[i] - a.coords[i],2);
        }
        return sqrt(rpta);
    }
};

template< typename T, int ndim >
struct Record {
    T object; //aqui el pokemon (aqui está su tamaño)
    Point<ndim> tuple; //coordenada donde está el pokemon
    Point<ndim> max_tuple;
    //Funcion para dar la dimencion del cubo que alberga el Record
    Record(T _object, Point<ndim> _tuple) {
        object = _object;
        tuple = _tuple;
        //////////////////////////
        max_tuple.coords[0] = tuple.coords[0] + object.height_m;
        max_tuple.coords[1] = tuple.coords[1] + object.hp;
        max_tuple.coords[2] = tuple.coords[2] + object.weight_kg_std;
        //////////////////////////
    }
    void save_txt(fstream& _newfile) {
        for (int i = 0; i < tuple.coords.size(); i ++) {
            _newfile << tuple.coords[i] << ' ';
        }
        for (int i = 0; i < max_tuple.coords.size(); i ++) {
            _newfile << max_tuple.coords[i] << ' ';
        }
        _newfile << endl;
    }
};

template <class T, int ndim>
Point<ndim> getMinPoint(vector< Record<T,ndim> > _records) {
    Point<ndim> minPoint = _records[0].tuple;
    for (int i = 1; i < _records.size(); i++) {
        for (int j = 0; j < ndim; j++){
            if (_records[i].tuple.coords[j] < minPoint.coords[j]) {
                minPoint.coords[j] = _records[i].tuple.coords[j];
            }
        }
    }
    return minPoint;
}
template <class T, int ndim>
Point<ndim> getMaxPoint(vector< Record<T,ndim> > _records) {
    Point<ndim> maxPoint = _records[0].max_tuple;
    for (int i = 1; i < _records.size(); i++) {
        for (int j = 0; j < ndim; j++){
            if (_records[i].max_tuple.coords[j] > maxPoint.coords[j]) {
                maxPoint.coords[j] = _records[i].max_tuple.coords[j];
            }
        }
    }
    return maxPoint;
}

template< typename T, int ndim >
class Node { //Region //Rectangle //MBR
private:
    Point<ndim> bottomLeft;
    Point<ndim> upperRight;

public:
    vector< Node<T,ndim>* > children;
    vector< Record<T,ndim> > records;
    int countRecords;
    bool isLeaf;  
    Node() {
        countRecords = 0;
        isLeaf = true;
    }
    bool insert_record(Record<T,ndim>& record){
        records.push_back(record);
        if (countRecords == 0) {
            bottomLeft = record.tuple;
            upperRight = record.max_tuple;
        }
        else {
            bottomLeft = getMinPoint(records);
            upperRight = getMaxPoint(records);
        }
        countRecords++;
        cout << "bottom -> ";
        bottomLeft.printP();
        cout << "upper -> ";
        upperRight.printP();
        cout << "Cant Records -> " << countRecords << endl;
        cout << "Record Inserted\n";
        return true;
    }
    
    //insert utilizado principalmente por RTree
    bool insert(pair< Node*, Node*>& nodes_p) {
        children.push_back(nodes_p.first);
        children.push_back(nodes_p.second);
        countRecords += 2;
        return true;
    }

    pair< Node*, Node*> split(){
        pair<Node<T,ndim>*, Node<T,ndim>*> rpta;

        double h = (bottomLeft.disEu(upperRight))/2;
        rpta.first = new Node<T,ndim>;              //creamos los nuevos nodos a los que se le hara split
        rpta.second = new Node<T,ndim>;

        for (int i = 0; i < countRecords; i++) {
            if (isLeaf) {
                if (records[i].tuple.disEu(bottomLeft) < h) {
                    rpta.first->insert_record(records[i]);
                }
                else {
                    rpta.second->insert_record(records[i]);
                }
            }
        }
        if (isLeaf) {
            isLeaf = false;
        }

        records.clear();        //Limpiamos los records, que ahora se almacenaran en las hojas
        countRecords = records.size();

        cout << "Doing a split and h -> " << h << " also cant records " << records.size() << endl;
        return rpta;
    }

    void save_txt(fstream& _newfile) {
        for (int i = 0; i < bottomLeft.coords.size(); i ++) {
            _newfile << bottomLeft.coords[i] << ' ';
        }
        for (int i = 0; i < upperRight.coords.size(); i ++) {
            _newfile << upperRight.coords[i] << ' ';
        }
        _newfile << endl;
    }
};

template< typename T, int ndim >
class RTree {
private:
    Node<T,ndim>* root;

public:
    RTree() {
        root = nullptr;
        cout << "Done!\n";
    };
    bool choose_Leaf( Record<T,ndim> &rec, Node<T,ndim>*& p) {
        if (p->isLeaf) {    //Si es un nodo hoja intentamos insertar
            if (p->countRecords < maxRecords) {
                return p->insert_record(rec);
            }
            else {  // Hacemos un Split
                p->insert_record(rec);      //Igualmente lo insertamos, para poder dividirlo                
                cout << "Split\n";
                p->insert(p->split());
                return false;
            }
        }
        else {      //cuando no es hoja, seguimos buscando por el arbol
            return false;
        }
    }
    void insert( Record<T,ndim> &rec ) {
        if (!root) {
            root =  new Node<T,ndim>;
        }
        Node<T,ndim>* p = root;
        if (choose_Leaf(rec, p)) { cout << "Inserted\n"; return; };
    };

    //Funciones para guardar los puntos 
    void move_tree(fstream& _newfile, Node<T,ndim>* p) {
        if(_newfile.is_open()) //checking whether the file is open  
            cout << "count records here -> " << p->countRecords << endl;
            p->save_txt(_newfile);
            if (p->isLeaf) {
                for (int i = 0; i < p->countRecords; i++) {
                    p->records[i].save_txt(_newfile);
                }
                //_newfile << leftBottom.x << ' ' << leftBottom.y << ' ' << leftBottom.z << ' ' << h << endl;
                return;
            }
            else {
                Node<T,ndim>* q = p;
                for (int i = 0; i < p->countRecords; i++) {
                    p = q->children[i];
                    move_tree(_newfile, p);
                }
            }
    }
    void write_Cubes(fstream& _newfile) {
        Node<T,ndim>* q = root;
        if(_newfile.is_open()) //checking whether the file is open
            move_tree(_newfile, q);
    }
};
#endif
