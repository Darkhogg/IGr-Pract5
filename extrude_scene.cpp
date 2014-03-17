#include "extrude_scene.hpp"


void igr::extrude_scene::on_begin () {
  cam.eye  = {10.f, 10.f, 10.f};
  cam.look = {0.f,  0.f,  0.f};
  cam.up   = {0.f,  1.f,  0.f};

  /* Setup the projection */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (float) window.getSize().x / (float) window.getSize().y, 1, 1000);

  /* Set up lighting */
  //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_COLOR_MATERIAL);
  glMaterialf(GL_FRONT, GL_SHININESS, 0.1f);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

   // Light0
  glEnable(GL_LIGHT0);
  GLfloat d[]={1.0, 1.0, 1.0, 1.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, d);
  GLfloat a[]={0.3f, 0.3f, 0.3f, 1.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, a);
  GLfloat p[]={25.0, 25.0, 0.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, p);
}


void igr::extrude_scene::on_update (float delta) {
  _vyaw   += _vyawspd   * delta;
  _vfar   += _vfarspd   * delta;
  _vpitch += _vpitchspd * delta;

  cam.eye = {
    _vfar * cos(_vyaw) * cos(_vpitch),
    _vfar * sin(_vpitch),
    _vfar * sin(_vyaw) * cos(_vpitch)
  };
}


void igr::extrude_scene::on_draw () {
  /* Clear the screen */
  glClearColor(0.15f, 0.175f, 0.2f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Draw axis to check everything ok */
  glBegin(GL_LINES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(-10.f, 0.f, 0.f);
    glVertex3f(+10.f, 0.f, 0.f);
    glVertex3f(10.f, +0.2f, 0.f);
    glVertex3f(10.f, -0.2f, 0.f);
    glVertex3f(10.f, 0.f, +0.2f);
    glVertex3f(10.f, 0.f, -0.2f);

    glColor3f(0.f, 0.8f, 0.f);
    glVertex3f(0.f, -10.f, 0.f);
    glVertex3f(0.f, +10.f, 0.f);
    glVertex3f(+0.2f, 10.f, 0.f);
    glVertex3f(-0.2f, 10.f, 0.f);
    glVertex3f(0.f, 10.f, +0.2f);
    glVertex3f(0.f, 10.f, -0.2f);

    glColor3f(0.f, 0.5f, 1.f);
    glVertex3f(0.f, 0.f, -10.f);
    glVertex3f(0.f, 0.f, +10.f);
    glVertex3f(+0.2f, 0.f, 10.f);
    glVertex3f(-0.2f, 0.f, 10.f);
    glVertex3f(0.f, +0.2f, 10.f);
    glVertex3f(0.f, -0.2f, 10.f);
  glEnd();

  glPolygonMode(GL_FRONT, GL_LINE);
  mesh box = mesh::make_aligned_box({1.f, 1.f, 1.f, 0.6f});
  box.gl_draw_immediate();

  /*
  glColor3f(1.f, 1.f, 1.f);
  glBegin(GL_LINE_LOOP);
  for (std::size_t i = 0u; i < 256; i++) {
    double t = (i * M_PI * 4) / 256.0;
    vec<double> v = _curve(t);
    glVertex3d(v.x, v.y, v.z);
  }
  glEnd();
  */
}


bool igr::extrude_scene::on_event (event_t event) {
  switch (event.type) {
    case sf::Event::Resized: {
      /* Viewport change */
      glViewport(0, 0, event.size.width, event.size.height);

      /* Setup the projection */
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(45, (float) event.size.width / (float) event.size.height, 1, 1000);


      return true;
    }

    case sf::Event::KeyPressed: {
      switch (event.key.code) {
        case sf::Keyboard::Key::A: {
          _vyawspd = 1.0;
          break;
        }
        case sf::Keyboard::Key::D: {
          _vyawspd = -1.0;
          break;
        }

        case sf::Keyboard::Key::W: {
          _vfarspd = -16.0;
          break;
        }
        case sf::Keyboard::Key::S: {
          _vfarspd = 16.0;
          break;
        }

        case sf::Keyboard::Key::Q: {
          _vpitchspd = 1.0;
          break;
        }
        case sf::Keyboard::Key::E: {
          _vpitchspd = -1.0;
          break;
        }

        default: {}
      }

      return true;
    }

    case sf::Event::KeyReleased: {
      switch (event.key.code) {
        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::D: {
          _vyawspd = 0.0;
          break;
        }
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::S: {
          _vfarspd = 0.0;
          break;
        }
        case sf::Keyboard::Key::Q:
        case sf::Keyboard::Key::E: {
          _vpitchspd = 0.0;
          break;
        }

        default: {}
      }
    }

    default:
      return false;
  }
}


/* Definition of main here */
int main () {
  igr::extrude_scene{}.run();
}