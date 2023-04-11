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

#include <CGAL/draw_voronoi_diagram_2.h>

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

	// if area is 0, return first
	if (max_area == 0)
		return faces_begin;

    return fh;
}

struct Utils{
	static bool present_in_set(std::set<Face_handle> *set, Face_handle val) {
		return set->find(val) != set->end();
	}

    static Face_handle get_cont_face(Halfedge_handle &he) {
        return he->opposite()->face();
    };

    static TPoint2 face_to_point(Face_handle fh) {
        return fh->dual()->point();
    }

	static void print_set(std::set<Face_handle> &set) {
		for (auto j = set.begin(); j != set.end(); j++) {
			std::cout << " - " << Utils::face_to_point(*j) << std::endl;
		}
	}
};

class IncFaces{
    std::vector<std::set<Face_handle>> caras;

    std::set<Face_handle>* get_last_level(int prev=1) {
		int pos = this->caras.size() - prev;
		if (pos < 0) {
			return &this->caras[0];
		}

        return &this->caras[this->caras.size() - prev];
    }

    void new_level(std::set<Face_handle> borde) {
        // se agrega un nuevo nivel
        this->caras.push_back(borde);
    }

public:

    IncFaces(Face_handle initial_face) {
        this->new_level(std::set<Face_handle> {initial_face});
    }

    void siguiente_nivel() {
        std::set<Face_handle> new_faces;

		auto last_level = this->get_last_level();
		auto prev_last_level = this->get_last_level(2);

        std::set<Face_handle> old_faces;
		old_faces.insert(last_level->begin(), last_level->end());
		old_faces.insert(prev_last_level->begin(), prev_last_level->end());

		auto borde = this->get_last_level();
		// std::cout << "borde " << this->level() << std::endl;
		// Utils::print_set(*borde);
		// std::cout << "fin borde" << std::endl;

        for (auto i = borde->begin(); i != borde->end(); i++) {
            Face_handle f = *i;

            Halfedge_handle he = f->halfedge();
            // para cada arista
            do {
                Face_handle fh = Utils::get_cont_face(he);
                // revisar cada cara contraria, que no esté en
                // Centros.centros
                if (!Utils::present_in_set(&old_faces, fh)
					&& !Utils::present_in_set(&new_faces, fh)) {
                    // si no esta, se agrega a Centros.nuevos
                    new_faces.insert(fh);
                }

                he = he->next();
            } while (he != f->halfedge());
        }

		// std::cout << "nuevos valores " << this->level() << std::endl;
		// Utils::print_set(new_faces);
		// std::cout << "fin nuevos valores " << this->level() << std::endl;


        // valor cambian Centros.nuevos por los nuevos encontrados
        this->new_level(new_faces);
    }

    bool change() {
        // si los nuevos estan vacios, significa que en la ultima ronda no hubo cambio
        return !this->get_last_level()->empty();
    }

    double level() {
        return this->caras.size();
    }

    double size() {
		double tam = 0;
        for (auto i = this->caras.begin(); i != this->caras.end(); i++) {
			tam += i->size();
        }
        return tam;
    }

    void print() {
        // esto imprime los centroides de las caras
        std::cout << "caras:" << std::endl;
        int a = 0;
		double tam = 0;
        for (auto i = this->caras.begin(); i != this->caras.end(); i++) {
			std::cout << a << std::endl;
			Utils::print_set(*i);
            a++;
			tam += i->size();
        }
		std::cout << "Contiene " << tam << " caras despues de " << this->level() << " rondas" << std::endl;
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
        vd.insert(TPoint2(dis(gen) * 10, dis(gen) * 10));
    }

    IncFaces inc_caras{
        get_greatest_face_area(vd.faces_begin(), vd.faces_end())
    };

    std::cout << "inicial: " << std::endl;
    inc_caras.print();

    for (int a = 0 ; a < rondas ; a++) {
        inc_caras.siguiente_nivel();
        // std::cout << "size: " << inc_caras.size() << std::endl;

        if (!inc_caras.change()) {
            std::cout << "no mas cambios. " << a << " rondas." << std::endl;
            break;
        }
    }

	inc_caras.print();

	CGAL::draw(vd);

    return 0;
}
