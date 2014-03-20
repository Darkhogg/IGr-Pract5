#include "extrude_scene.hpp"

#define TUBE_SIDES 24
#define TUBE_SECTIONS 256


void igr::extrude_scene::on_begin () {
  cam.eye  = {10.f, 10.f, 10.f};
  cam.look = {0.f,  0.f,  0.f};
  cam.up   = {0.f,  1.f,  0.f};

  /* Setup the projection */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (float) window.getSize().x / (float) window.getSize().y, 0.1, 1000);

  /* Set up lighting */
  //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_COLOR_MATERIAL);
  glMaterialf(GL_FRONT, GL_SHININESS, 0.1f);


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  glEnable(GL_MULTISAMPLE);

   // Light0
  glEnable(GL_LIGHT0);
  GLfloat d[]={1.0, 1.0, 1.0, 1.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, d);
  GLfloat a[]={0.3f, 0.3f, 0.3f, 1.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, a);
  GLfloat p[]={25.0, 25.0, 0.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, p);

  /* Initial meshes */
  _box = mesh::make_aligned_box({0.1f, 0.5f, 0.9f});


  /* Construct the tube polygon */
  mesh poly;
  for (int i = 0; i < TUBE_SIDES; ++i) {
    double ang = 2.0 * M_PI * (double) i / (double) TUBE_SIDES;
    poly.add_vertex(
      {0.7 * sin(ang), 0.7 * cos(ang), 0},
      {sin(ang), cos(ang), 0},
      {0.1f, 0.4f, 0.9f, 1.f},
      {}
    );
    if (i > 1) {
      poly.add_face(0, i - 1, i);
    }
  }

  for (int i = 0; i < TUBE_SECTIONS; ++i) {
    double pos = 4 * M_PI * i / (double) TUBE_SECTIONS;
    matr<double> m;

    vec<double> t = _dcurve(pos).normalized();
    vec<double> b = _dcurve(pos).cross(_ddcurve(pos)).normalized();
    vec<double> n = b.cross(t).normalized();
    vec<double> c = _curve(pos);


    m(0, 0) = n.x; m(1, 0) = n.y; m(2, 0) = n.z; m(3, 0) = 0;
    m(0, 1) = b.x; m(1, 1) = b.y; m(2, 1) = b.z; m(3, 1) = 0;
    m(0, 2) = t.x; m(1, 2) = t.y; m(2, 2) = t.z; m(3, 2) = 0;
    m(0, 3) = c.x; m(1, 3) = c.y; m(2, 3) = c.z; m(3, 3) = 1;

    auto trans = poly.transformed(m);

    for (std::size_t j = 0; j < TUBE_SIDES; ++j) {
      auto tv = trans.vertices[j];
      _tube.add_vertex(tv.point, tv.normal, tv.color, {});

      std::size_t j1  = (j + 1) % TUBE_SIDES;
      std::size_t k00 = j  + i * TUBE_SIDES;
      std::size_t k10 = j1 + i * TUBE_SIDES;
      std::size_t k01 = j  + ((i + 1) % TUBE_SECTIONS) * TUBE_SIDES;
      std::size_t k11 = j1 + ((i + 1) % TUBE_SECTIONS) * TUBE_SIDES;
      _tube.add_face(k00, k10, k01);
      _tube.add_face(k01, k10, k11);
      std::cout << k00 << ", " << k01 << ", " << k10 << ", " << k11 << std::endl;
    }
  }
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

  _t += delta;
}


void igr::extrude_scene::on_draw () {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  /* Clear the screen */
  glClearColor(0.15f, 0.175f, 0.2f, 1.f);
  glClearDepth(1.0f);
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

  vec<double> c   = _curve(_t);
  vec<double> dc  = _dcurve(_t);
  vec<double> ddc = _ddcurve(_t);

  _tube.gl_draw_immediate();

  glMatrixMode(GL_MODELVIEW);  
  glPushMatrix();
  glTranslated(c.x, c.y, c.z);

  vec<double> dir {0.0, 0.0, 1.0};
  vec<double> n = dir.cross(dc).normalized();

  glRotated(180 * dir.angle(dc) / M_PI, n.x, n.y, n.z);

  _box.gl_draw_immediate();
  glBegin(GL_LINES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(1.f, 0.f, 0.f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 1.f, 0.f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, 1.f);
  glEnd();
  glPopMatrix();

  glColor3f(1.f, 1.f, 1.f);
  glBegin(GL_LINE_LOOP);
  for (std::size_t i = 0u; i < 256; i++) {
    double t = (i * M_PI * 4) / 256.0;
    vec<double> v = _curve(t);
    glVertex3d(v.x, v.y, v.z);
  }
  glEnd();
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