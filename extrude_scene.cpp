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
  _box = mesh::make_aligned_box({0.9f, 0.9f, 0.9f});
  _box.transform(matr<double>::make_scalation({0.75, 0.75, 1.0}));
  _box.transform(matr<double>::make_translation({0.0, -0.1, 0.0}));

  _wheels[0] = mesh::make_aligned_cylinder({0.7f, 0.7f, 0.7f}, 32);
  _wheels[0].transform(matr<double>::make_scalation({0.45f, 0.45f, 0.25f}));
  _wheels[0].transform(matr<double>::make_rotation_y(M_PI_2));
  _wheels[0].transform(matr<double>::make_translation({0.27, 0.3, 0.35}));

  _wheels[1] = mesh::make_aligned_cylinder({0.7f, 0.7f, 0.7f}, 32);
  _wheels[1].transform(matr<double>::make_scalation({0.45f, 0.45f, 0.25f}));
  _wheels[1].transform(matr<double>::make_rotation_y(M_PI_2));
  _wheels[1].transform(matr<double>::make_translation({-0.27, 0.3, 0.35}));

  _wheels[2] = mesh::make_aligned_cylinder({0.7f, 0.7f, 0.7f}, 32);
  _wheels[2].transform(matr<double>::make_scalation({0.45f, 0.45f, 0.25f}));
  _wheels[2].transform(matr<double>::make_rotation_y(M_PI_2));
  _wheels[2].transform(matr<double>::make_translation({0.27, 0.3, -0.35}));

  _wheels[3] = mesh::make_aligned_cylinder({0.7f, 0.7f, 0.7f}, 32);
  _wheels[3].transform(matr<double>::make_scalation({0.45f, 0.45f, 0.25f}));
  _wheels[3].transform(matr<double>::make_rotation_y(M_PI_2));
  _wheels[3].transform(matr<double>::make_translation({-0.27, 0.3, -0.35}));


  /* Construct the tube polygon */
  mesh poly;
  for (int i = 0; i < TUBE_SIDES; ++i) {
    double ang = 2.0 * M_PI * (double) i / (double) TUBE_SIDES;
    poly.add_vertex(
      {0.7 * sin(ang), 0.7 * cos(ang), 0},
      {sin(ang), cos(ang), 0},
      {}, {}
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
      double cang = 2 * M_PI * i / (double) TUBE_SECTIONS;
      color rbc = {
        (float) ((cos(cang) + 1) / 2),
        (float) ((cos(cang + M_PI / 1.5) + 1) / 2),
        (float) ((cos(cang + 2 * M_PI / 1.5) + 1) / 2),
        1.f
      };
      _tube.add_vertex(tv.point, tv.normal, rbc, {});

      std::size_t j1  = (j + 1) % TUBE_SIDES;
      std::size_t k00 = j  + i * TUBE_SIDES;
      std::size_t k10 = j1 + i * TUBE_SIDES;
      std::size_t k01 = j  + ((i + 1) % TUBE_SECTIONS) * TUBE_SIDES;
      std::size_t k11 = j1 + ((i + 1) % TUBE_SECTIONS) * TUBE_SIDES;
      _tube.add_face(k00, k10, k01);
      _tube.add_face(k01, k10, k11);
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

  /* Axis Z */
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y)) {
    _t += delta;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::H)) {
    _t -= delta;
  }


  /* Axis X */
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
    _tubetrans = matr<double>::make_rotation_x(delta) * _tubetrans;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::T)) {
    _tubetrans = matr<double>::make_rotation_x(-delta) * _tubetrans;
  }

  /* Axis Y */
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) {
    _tubetrans = matr<double>::make_rotation_y(delta) * _tubetrans;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G)) {
    _tubetrans = matr<double>::make_rotation_y(-delta) * _tubetrans;
  }

  /* Axis Z */
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V)) {
    _tubetrans = matr<double>::make_rotation_z(delta) * _tubetrans;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::B)) {
    _tubetrans = matr<double>::make_rotation_z(-delta) * _tubetrans;
  }
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

  glPolygonMode(GL_FRONT_AND_BACK, _tubelines ? GL_LINE : GL_FILL);
  mesh tube = _tube.transformed(_tubetrans);

  tube.gl_draw_immediate();
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N)) {
    tube.gl_draw_normals();
  }

  glMatrixMode(GL_MODELVIEW);  
  glPushMatrix();

  mesh trbox {_box};
  std::array<mesh, 4> trwheels = _wheels;
  matr<double> m;

  vec<double> t = _dcurve(_t).normalized();
  vec<double> b = _dcurve(_t).cross(_ddcurve(_t)).normalized();
  vec<double> n = b.cross(t).normalized();
  vec<double> c = _curve(_t);

  m(0, 0) = n.x; m(1, 0) = n.y; m(2, 0) = n.z; m(3, 0) = 0;
  m(0, 1) = b.x; m(1, 1) = b.y; m(2, 1) = b.z; m(3, 1) = 0;
  m(0, 2) = t.x; m(1, 2) = t.y; m(2, 2) = t.z; m(3, 2) = 0;
  m(0, 3) = c.x; m(1, 3) = c.y; m(2, 3) = c.z; m(3, 3) = 1;

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  trbox.transform(m);
  trbox.transform(_tubetrans);
  trbox.gl_draw_immediate();
  for (auto trwheel : trwheels) {
    trwheel.transform(m);
    trwheel.transform(_tubetrans);
    trwheel.gl_draw_immediate();
  }

  glPopMatrix();
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


        case sf::Keyboard::Key::Z: {
          _tubelines = true;
          break;
        }
        case sf::Keyboard::Key::X: {
          _tubelines = false;
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