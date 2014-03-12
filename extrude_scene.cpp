#include "extrude_scene.hpp"


void igr::extrude_scene::on_begin () {
  cam.eye  = {10.f, 10.f, 10.f};
  cam.look = {0.f,  0.f,  0.f};
  cam.up   = {0.f,  1.f,  0.f};

  /* Setup the projection */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (float) window.getSize().x / (float) window.getSize().y, 1, 1000);
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
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

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
}


bool igr::extrude_scene::on_event (event_t event) {
  switch (event.type) {
    case sf::Event::Resized: {

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