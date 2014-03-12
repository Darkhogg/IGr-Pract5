#ifndef __EXTRUDE_SCENE__HPP__
#define __EXTRUDE_SCENE__HPP__

#include "engine/common.h"
#include "engine/scene.hpp"

namespace igr {

  class extrude_scene : public scene<extrude_scene> {
      double _vyaw,   _vyawspd;
      double _vpitch, _vpitchspd;
      double _vfar,   _vfarspd;

    public:
      extrude_scene ()
        : _vyaw(1.0), _vpitch(0.5), _vfar(16.0)
      {}

      void on_begin ();
      void on_draw ();
      void on_update (float delta);
      bool on_event (event_t event);
  };

}

#endif