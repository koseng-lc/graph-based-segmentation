#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>

#include <random>

using namespace std;

using namespace cv;

int k = 700;
size_t thresh_size = 50;

class Graph{
public:
    class Vertex{
    public:
        Vertex(){
            this->x = x;
            this->y = y;
        }
        Vertex(int _x,int _y){
            this->x = _x;
            this->y = _y;
        }
        int x,y;
    };

    class Edge{
    public:
        Edge(){}
        Edge(Vertex _v1, Vertex _v2,float _weight){
            this->v1 = _v1;
            this->v2 = _v2;
            this->weight = _weight;
        }
        Vertex v1;
        Vertex v2;
        float weight;
    };

    vector<Edge > edges;
};

class Component{
public:
    Component(){
        this->color = Vec3b(rand()%255,rand()%255,rand()%255);
//        cout << color << endl;
        this->int_diff=0;
    }
    vector<Graph::Vertex> element;
    int idx;
    float int_diff;
    Vec3b color;
};

float tau(float sz_c){
    return float(k)/sz_c;
}

float threeChannelWeight(int ch1,int ch2,int ch3){
    return sqrt(ch1*ch1 + ch2*ch2 + ch3*ch3);
}

bool compareFunc(Graph::Edge e1, Graph::Edge e2){
    return e1.weight < e2.weight;
}

int main(){

//    Mat input_img = imread("/media/koseng/563C3F913C3F6ADF/Media/1.jpg");

    VideoCapture vc("/media/koseng/4A7AE1C07AE1A941/alfarobi/recorded/video3.avi");

    if(!vc.isOpened()){
        cout << "Video not opened !!!" << endl;
        return -1;
    }

    while(1){
        Mat input_img;
        vc >> input_img;

        resize(input_img,input_img,Size(80,60));

        Graph graph;

        int num_rows = input_img.rows;
        int num_cols = input_img.cols;

        double dT = getTickCount();

        for(int i=0;i<num_rows;i++){
            Vec3b *ctr_ptr = input_img.ptr<Vec3b>(i);
            Vec3b *btm_ptr = input_img.ptr<Vec3b>(i<num_rows?i+1:i);
            for(int j=0;j<num_cols;j++){
                Graph::Vertex ctr(j,i);
                Graph::Vertex nb1(j+1,i);
                Graph::Vertex nb2(j+1,i+1);
                Graph::Vertex nb3(j,i+1);
                Graph::Vertex nb4(j-1,i+1);

                if(nb1.x < num_cols)
                    graph.edges.push_back(Graph::Edge(ctr,nb1,threeChannelWeight(ctr_ptr[j][0] - ctr_ptr[nb1.x][0],
                            ctr_ptr[j][1] - ctr_ptr[nb1.x][1], ctr_ptr[j][2] - ctr_ptr[nb1.x][2])));

                if(nb2.x < num_cols && nb2.y < num_rows)
                    graph.edges.push_back(Graph::Edge(ctr,nb2,threeChannelWeight(ctr_ptr[j][0] - btm_ptr[nb2.x][0],
                            ctr_ptr[j][1] - btm_ptr[nb2.x][1], ctr_ptr[j][2] - btm_ptr[nb2.x][2])));

                if(nb3.y < num_rows)
                    graph.edges.push_back(Graph::Edge(ctr,nb3,threeChannelWeight(ctr_ptr[j][0] - btm_ptr[nb3.x][0],
                            ctr_ptr[j][1] - btm_ptr[nb3.x][1], ctr_ptr[j][2] - btm_ptr[nb3.x][2])));

                if(nb4.x >= 0 && nb4.y < num_rows)
                    graph.edges.push_back(Graph::Edge(ctr,nb4,threeChannelWeight(ctr_ptr[j][0] - btm_ptr[nb4.x][0],
                            ctr_ptr[j][1] - btm_ptr[nb4.x][1], ctr_ptr[j][2] - btm_ptr[nb4.x][2])));

            }
        }

        sort(graph.edges.begin(),graph.edges.end(),compareFunc);

        dT = (getTickCount() - dT)/getTickFrequency();

        cout << dT << endl;

    //    for(size_t i=0;i<graph.edges.size();i++){
    //        cout << graph.edges[i].weight << endl;
    ////        cout << " V1 : " << graph.edges[i].v1.x << " , " << graph.edges[i].v1.y << endl;
    ////        cout << " V2 : " << graph.edges[i].v2.x << " , " << graph.edges[i].v2.y << endl;
    ////        cout << graph.edges[i].weight << endl;
    //        Graph::Edge e1;
    //        e1.v1 = graph.edges[i].v1;
    //        e1.v2 = graph.edges[i].v2;
    //        e1.weight = graph.edges[i].weight;
    //        for(size_t j=i;j<graph.edges.size();j++){
    //            if(graph.edges[i].weight > graph.edges[j].weight){
    //                graph.edges[i] = graph.edges[j];
    //                graph.edges[j] = e1;
    //            }
    //        }
    //    }

        vector<int > idx_comp(num_rows*num_cols);

        vector<Component > components(num_rows*num_cols);

        for(size_t i=0;i<idx_comp.size();i++){
            idx_comp[i] = i;
            components[i].idx = i;
            components[i].element.push_back(Graph::Vertex(i%num_cols,i/num_cols));
    //        cout << components[i].element.
        }

        cout << "NUM of EDGES : " << graph.edges.size() << endl;

        for(size_t i=0;i<graph.edges.size();i++){
            int idx1 = graph.edges[i].v1.x + graph.edges[i].v1.y*num_cols;
            int idx2 = graph.edges[i].v2.x + graph.edges[i].v2.y*num_cols;
            int comp1 = idx_comp[idx1];
            int comp2 = idx_comp[idx2];

            if(comp1 != comp2 &&
                    graph.edges[i].weight <= fmin(components[comp1].int_diff + tau(components[comp1].element.size()), components[comp2].int_diff + tau(components[comp2].element.size()))){

                 components[comp1].element.insert(components[comp1].element.end(),components[comp2].element.begin(),
                                                  components[comp2].element.end());
                 for(size_t j=0;j<components[comp2].element.size();j++){
                     Graph::Vertex temp_vert = components[comp2].element[j];
    //                 cout << temp_vert.x << " , " << temp_vert.y << endl;
                     int temp_idx = temp_vert.x + temp_vert.y*num_cols;
                     idx_comp[temp_idx] = comp1;
                 }

                 components[comp2].element.clear();
                 components[comp1].int_diff = graph.edges[i].weight;
            }
        }

        for(size_t i=0;i<graph.edges.size();i++){
            int idx1 = graph.edges[i].v1.x + graph.edges[i].v1.y*num_cols;
            int idx2 = graph.edges[i].v2.x + graph.edges[i].v2.y*num_cols;
            int comp1 = idx_comp[idx1];
            int comp2 = idx_comp[idx2];
    //        cout << comp1 << " , " << comp2 << endl;
            if(comp1 != comp2 &&
                    (components[comp1].element.size() < thresh_size || components[comp2].element.size() < thresh_size)){

                components[comp1].element.insert(components[comp1].element.end(),components[comp2].element.begin(),
                                                 components[comp2].element.end());
                for(size_t j=0;j<components[comp2].element.size();j++){
                    Graph::Vertex temp_vert = components[comp2].element[j];
                    int temp_idx = temp_vert.x + temp_vert.y*num_cols;
                    idx_comp[temp_idx] = comp1;
                }

                components[comp2].element.clear();
            }
        }

        Mat out = Mat::zeros(input_img.size(),CV_8UC3);

    //    for(size_t i=0;i<idx_comp.size();i++){
    //        cout << idx_comp[i] << endl;
    //    }

        for(int i=0;i<num_rows;i++){
            Vec3b *out_ptr = out.ptr<Vec3b>(i);
            for(int j=0;j<num_cols;j++){
    //            cout << idx_comp[j+i*num_cols] << endl;
                out_ptr[j] = components[idx_comp[j+i*num_cols]].color;
            }
        }

        Mat dx,dy,mag,ang;
        Sobel(out,dx,CV_32F,1,0,3);
        Sobel(out,dy,CV_32F,0,1,3);
        cartToPolar(dx,dy,mag,ang);

    //    vector<Point> edges;

    //    Canny(out,out,edges,50,100);
    //    Mat dx,dy,mag,ang;
    //    Sobel(out,dx,CV_8U,1,0,3);
    //    Sobel(out,dy,CV_8U,0,1,3);
    //    cartToPolar(dx,dy,mag,ang);
        resize(out,out,Size(640,480));
        imshow("INPUT",input_img);
        imshow("OUTPUT",out);
        imshow("GRAD",mag);

        if(waitKey(0)==27)break;

    }

//    resize(input_img,input_img,Size(320,240));


    return 0;
}
