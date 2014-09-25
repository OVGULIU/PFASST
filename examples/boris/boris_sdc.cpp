#include <memory>

#include <pfasst.hpp>
#include <pfasst/sdc.hpp>

#include "particle_3d.hpp"
#include "simple_physics.hpp"
#include "boris_sweeper.hpp"

error_map<double> run_boris_sdc()
{
  pfasst::SDC<> sdc;

  const size_t nsteps     = 100;
  const double dt         = 0.01;
  const size_t nnodes     = 2;
//   const size_t nparticles = 1;
  const size_t niters     = 2;

  const double mass = 1.0;
  const double charge = 1.0;

  const double epsilon = -1.0;

  auto nodes      = pfasst::compute_nodes<double>(nnodes, pfasst::QuadratureType::GaussLobatto);
  auto factory    = make_shared<Particle3DFactory<double, double>>(mass, charge);

  typedef SimplePhysicsEnergyOperator<double, double, Particle3DEncapsulation,
                                      IdealQuadrupolePotential, ConstantMagneticField> energy_operator_type;
  energy_operator_type::e_field_type e_field(epsilon);
  energy_operator_type::b_field_type b_field;
  energy_operator_type e_operator(e_field, b_field, epsilon);
  auto sweeper    = make_shared<BorisSweeper<double, double, energy_operator_type>>(e_operator, epsilon);

  sweeper->set_energy_operator(e_operator);
  sweeper->set_nodes(nodes);
  sweeper->set_factory(factory);

  sdc.add_level(sweeper);
  sdc.set_duration(0.0, nsteps*dt, dt, niters);
  sdc.setup();

  auto p0 = dynamic_pointer_cast<Particle3DEncapsulation<double, double>>(sweeper->get_state(0));
  p0->pos().x = 10;
  p0->vel().u = 100;
  p0->vel().w = 100;

  cout << "Initial Particle: " << *(dynamic_pointer_cast<Particle3DEncapsulation<double, double>>(sweeper->get_state(0))) << endl;
  sdc.run();

  return sweeper->get_errors();
}


int main(int /* argn */, char** /* argc */)
{
//   cout << scientific;
  cout << fixed;
  cout.precision(6);
  run_boris_sdc();
}
