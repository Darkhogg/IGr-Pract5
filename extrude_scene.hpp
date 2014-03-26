/**
 * @author Daniel Escoz Solana
 */
#ifndef __EXTRUDE_SCENE__HPP__
#define __EXTRUDE_SCENE__HPP__

#include "engine/common.h"
#include "engine/scene.hpp"
#include "engine/color.hpp"
#include "engine/mesh.hpp"

#include <functional>
#include <array>

namespace igr {

  class extrude_scene : public scene<extrude_scene> {
      double _vyaw,   _vyawspd;
      double _vpitch, _vpitchspd;
      double _vfar,   _vfarspd;

      std::function<vec<double>(double)> _curve, _dcurve, _ddcurve;
      double _t;

      mesh _box;

      mesh _tube;
      std::array<mesh, 4> _wheels;
      std::array<matr<double>, 4> _wheeltrans;
      matr<double> _tubetrans;

      bool _tubelines;

    public:
      extrude_scene ()
        : scene{"Extruded Roller Coaster"},
          _vyaw{1.0}, _vpitch{0.5}, _vfar{16.0}
      {
        _curve = [](double t) -> vec<double>{
          return {
            3.0 * cos(t),
            2.0 * cos(1.5 * t),
            3.0 * sin(t),
            category::point
          };
        };

        _dcurve = [](double t) -> vec<double>{
          return {
            -3.0 * sin(t),
            -3.0 * sin(1.5 * t),
            3.0 * cos(t),
            category::point
          };
        };

        _ddcurve = [](double t) -> vec<double>{
          return {
            -3.0 * cos(t),
            -4.5 * cos(1.5 * t),
            -3.0 * sin(t),
            category::point
          };
        };
      }

      void on_begin ();
      void on_draw ();
      void on_update (float delta);
      bool on_event (event_t event);
  };

}

#endif