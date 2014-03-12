#ifndef __EXTRUDE_SCENE__HPP__
#define __EXTRUDE_SCENE__HPP__

#include "engine/common.h"
#include "engine/scene.hpp"

namespace igr {

  class extrude_scene : public scene<extrude_scene> {

    public:
      void on_begin ();
      void on_draw ();
      bool on_event (event_t event);
  };

}

#endif