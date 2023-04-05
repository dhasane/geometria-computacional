#include <iostream>
#include <vector>
#include <deque>
#include <random>

// includes for defining the Voronoi diagram adaptor
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/draw_triangulation_2.h>

using K  = CGAL::Exact_predicates_inexact_constructions_kernel;
using DT = CGAL::Delaunay_triangulation_2<K>;
using AT = CGAL::Delaunay_triangulation_adaptation_traits_2<DT>;
using AP = CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT>;
using VD = CGAL::Voronoi_diagram_2<DT,AT,AP>;

using Face     = VD::Face;
using Halfedge = VD::Halfedge;
using Face_handle     = VD::Face_handle;
using Halfedge_handle = VD::Halfedge_handle;

using TPoint2 = K::Point_2;
using TLine2  = K::Line_2;

double area_green(std::vector<TPoint2>::iterator start, std::vector<TPoint2>::iterator end)
{
    int move = 1;
    double area = 0;
    for(std::vector<TPoint2>::iterator i=start ; i != std::next(end, - move) ; i = std::next(i, move))
    {
        double x = i->x();
        double y = i->y();

        double xd = std::next(i, move)->x();
        double yd = std::next(i, move)->y();

        area += ( ( x * yd ) - ( y * xd ) );
    }

	area += ( ( (end-move)->x() * start->y() ) - ( (end-move)->y() * start->x() ) );

    return area / 2;
}

double calculate_face_area (Face &a) {
    std::vector<TPoint2> pts;
    Halfedge he = *a.halfedge();

    bool check_area = true;
    while(check_area && he.next() != a.halfedge())
    {
        // siempre poner el punto anterior
        if (he.previous()->has_target()) {
            pts.push_back(he.previous()->target()->point());
            he = *he.next();
        } else {
            check_area = false;
        }
    }
    if (check_area) {
        return area_green(pts.begin(), pts.end());
    }
    return 0;
};

int get_faces_areas(VD::Face_iterator faces_begin, VD::Face_iterator faces_end) {
    int pos_max = 0;
    double max_area = 0;

    int pos = 0;
    for (auto it = faces_begin; it != faces_end; it++) {
        double area = calculate_face_area(*it);
        std::cout << pos << ": " << area << std::endl;
        if (max_area < area) {
            max_area = area;
            pos_max = pos;
        }
        pos++;
    }

    std::cout << "max area: " << max_area << " pos: " << pos << std::endl;
    return pos_max;
}

Face_handle get_greatest_face_area(VD::Face_iterator faces_begin, VD::Face_iterator faces_end) {
    Face_handle fh;
    double max_area = 0;

    for (auto it = faces_begin; it != faces_end; it++) {
        double area = calculate_face_area(*it);
        // std::cout << pos << ": " << area << std::endl;
        if (max_area < area) {
            max_area = area;
            fh = it;
        }
    }

    std::cout << "max area: " << max_area << " pos: [" << *fh->dual() << "]" << std::endl;
    return fh;
}

class IncFaces{
    std::set<Face_handle> caras;
    std::vector<Face_handle> borde;

    bool present(Face_handle fh) {
        return this->caras.find(fh) != this->caras.end();
    }

    Face_handle get_cont_face(Halfedge_handle &he) {
        return he->opposite()->face();
    };

    void merge() {
        for (auto i = this->borde.begin(); i != this->borde.end(); i++) {
            this->caras.insert(*i);
        }
    }

    void replace_new(std::vector<Face_handle> new_faces) {
        this->borde = new_faces;
    }

    TPoint2 face_to_point(Face_handle fh) {
        // a delaunay
        return fh->dual()->point();
    }

public:

    IncFaces(Face_handle initial_face) {
        this->caras.insert(initial_face);
        this->borde.push_back(initial_face);
    }

    void siguiente_nivel() {
        std::vector<Face_handle> new_faces;
        for (auto i = this->borde.begin(); i != this->borde.end(); i++) {
            Face_handle f = *i;

            Halfedge_handle he = f->halfedge();
            // para cada arista
            while (he->next() != f->halfedge()) {
                Face_handle fh = get_cont_face(he);
                // revisar cada cara contraria, que no esté en
                // Centros.centros
                if (!this->present(fh)) {
                    // si no esta, se agrega a Centros.nuevos
                    new_faces.push_back(fh);

                    // para evitar volver a revisar valores que esten entre los nuevos
                    this->caras.insert(fh);
                }
                // de lo contrario, no se hace nada
                he = he->next();
            }
        }
        // valor cambian Centros.nuevos por los nuevos encontrados
        this->replace_new(new_faces);
    }

    bool change() {
        // si los nuevos estan vacios, significa que en la ultima ronda no hubo cambio
        return !this->borde.empty();
    }

    double size() {
        return this->caras.size();
    }

    void print() {
        // esto imprime los centroides de las caras
        std::cout << "caras:" << std::endl;
        for (auto i = this->caras.begin(); i != this->caras.end(); i++) {
            std::cout << "- " << this->face_to_point(*i) << std::endl;
        }
        std::cout << "borde:" << std::endl;
        for (auto i = this->borde.begin(); i != this->borde.end(); i++) {
            std::cout << "- " << this->face_to_point(*i) << std::endl;
        }
    }
};

int main(int argc, char** argv)
{
    int vd_tam = argc >= 2 ? std::atoi(argv[1]) : 1000;
    int rondas = argc >= 3 ? std::atoi(argv[2]) : 15;

    VD vd;

    std::random_device r;
    std::default_random_engine gen(r());
    std::uniform_real_distribution<K::FT> dis(-1, 1);

    for (int a = 0; a < vd_tam; ++a) {
        vd.insert(TPoint2(dis(gen), dis(gen)));
    }

    IncFaces inc_caras{
        get_greatest_face_area(vd.faces_begin(), vd.faces_end())
    };

    std::cout << "inicial: " << std::endl;
    inc_caras.print();

    for (int a = 0 ; a < rondas ; a++) {
        inc_caras.siguiente_nivel();
        inc_caras.print();
        std::cout << "size: " << inc_caras.size() << std::endl;

        if (!inc_caras.change()) {
            std::cout << "no mas cambios. " << a << " rondas." << std::endl;
            break;
        }
    }

    return 0;
}
