//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "InteractionIntegral.h"
#include "MooseMesh.h"
#include "RankTwoTensor.h"
#include "Conversion.h"
#include "libmesh/string_to_enum.h"
#include "libmesh/quadrature.h"
#include "DerivativeMaterialInterface.h"
#include "libmesh/utility.h"

registerMooseObject("TensorMechanicsApp", InteractionIntegral);

MooseEnum
InteractionIntegral::qFunctionType()
{
  return MooseEnum("Geometry Topology", "Geometry");
}

MooseEnum
InteractionIntegral::sifModeType()
{
  return MooseEnum("KI KII KIII T", "KI");
}

InputParameters
InteractionIntegral::validParams()
{
  InputParameters params = ElementVectorPostprocessor::validParams();
  params.addRequiredCoupledVar(
      "displacements",
      "The displacements appropriate for the simulation geometry and coordinate system");
  params.addCoupledVar("temperature",
                       "The temperature (optional). Must be provided to correctly compute "
                       "stress intensity factors in models with thermal strain gradients.");
  params.addRequiredParam<UserObjectName>("crack_front_definition",
                                          "The CrackFrontDefinition user object name");
  MooseEnum position_type("Angle Distance", "Distance");
  params.addParam<MooseEnum>(
      "position_type",
      position_type,
      "The method used to calculate position along crack front.  Options are: " +
          position_type.getRawNames());
  params.addRequiredParam<Real>(
      "K_factor", "Conversion factor between interaction integral and stress intensity factor K");
  params.addParam<unsigned int>("symmetry_plane",
                                "Account for a symmetry plane passing through "
                                "the plane of the crack, normal to the specified "
                                "axis (0=x, 1=y, 2=z)");
  params.addRequiredParam<Real>("poissons_ratio", "Poisson's ratio for the material.");
  params.addRequiredParam<Real>("youngs_modulus", "Young's modulus of the material.");
  params.set<bool>("use_displaced_mesh") = false;
  params.addRequiredParam<unsigned int>("ring_index", "Ring ID");
  params.addParam<MooseEnum>("q_function_type",
                             InteractionIntegral::qFunctionType(),
                             "The method used to define the integration domain. Options are: " +
                                 InteractionIntegral::qFunctionType().getRawNames());
  params.addRequiredParam<MooseEnum>("sif_mode",
                                     InteractionIntegral::sifModeType(),
                                     "Stress intensity factor to calculate. Choices are: " +
                                         InteractionIntegral::sifModeType().getRawNames());
  params.addParam<MaterialPropertyName>("eigenstrain_gradient",
                                        "Material defining gradient of eigenstrain tensor");
  params.addParam<MaterialPropertyName>("body_force", "Material defining body force");
  params.addClassDescription(
      "Computes the interaction integral, which is used to compute various "
      "fracture mechanics parameters at a crack tip, including KI, KII, KIII, "
      "and the T stress.");
  return params;
}

InteractionIntegral::InteractionIntegral(const InputParameters & parameters)
  : ElementVectorPostprocessor(parameters),
    _ndisp(coupledComponents("displacements")),
    _crack_front_definition(&getUserObject<CrackFrontDefinition>("crack_front_definition")),
    _treat_as_2d(false),
    _stress(getMaterialPropertyByName<RankTwoTensor>("stress")),
    _strain(getMaterialPropertyByName<RankTwoTensor>("elastic_strain")),
    _fe_vars(getCoupledMooseVars()),
    _fe_type(_fe_vars[0]->feType()),
    _grad_disp(3),
    _has_temp(isCoupled("temperature")),
    _grad_temp(_has_temp ? coupledGradient("temperature") : _grad_zero),
    _K_factor(getParam<Real>("K_factor")),
    _has_symmetry_plane(isParamValid("symmetry_plane")),
    _poissons_ratio(getParam<Real>("poissons_ratio")),
    _youngs_modulus(getParam<Real>("youngs_modulus")),
    _ring_index(getParam<unsigned int>("ring_index")),
    _total_deigenstrain_dT(hasMaterialProperty<RankTwoTensor>("total_deigenstrain_dT")
                               ? &getMaterialProperty<RankTwoTensor>("total_deigenstrain_dT")
                               : nullptr),
    _q_function_type(getParam<MooseEnum>("q_function_type").getEnum<QMethod>()),
    _position_type(getParam<MooseEnum>("position_type").getEnum<PositionType>()),
    _sif_mode(getParam<MooseEnum>("sif_mode").getEnum<SifMethod>()),
    _x(declareVector("x")),
    _y(declareVector("y")),
    _z(declareVector("z")),
    _position(declareVector("id")),
    _interaction_integral(declareVector("II_" + Moose::stringify(getParam<MooseEnum>("sif_mode")) +
                                        "_" + Moose::stringify(_ring_index))),
    _eigenstrain_gradient(nullptr),
    _body_force(nullptr)
{
  if (_has_temp && !_total_deigenstrain_dT)
    mooseError("InteractionIntegral Error: To include thermal strain term in interaction integral, "
               "must both couple temperature in DomainIntegral block and compute "
               "total_deigenstrain_dT using ThermalFractureIntegral material model.");

  // plane strain
  _kappa = 3.0 - 4.0 * _poissons_ratio;
  _shear_modulus = _youngs_modulus / (2.0 * (1.0 + _poissons_ratio));

  // Checking for consistency between mesh size and length of the provided displacements vector
  if (_ndisp != _mesh.dimension())
    paramError("displacements",
               "InteractionIntegral Error: number of variables supplied in 'displacements' must "
               "match the mesh dimension.");

  // fetch gradients of coupled variables
  for (std::size_t i = 0; i < _ndisp; ++i)
    _grad_disp[i] = &coupledGradient("displacements", i);

  // set unused dimensions to zero
  for (std::size_t i = _ndisp; i < 3; ++i)
    _grad_disp[i] = &_grad_zero;

  if (isParamValid("eigenstrain_gradient"))
  {
    _eigenstrain_gradient = &getMaterialProperty<RankThreeTensor>("eigenstrain_gradient");
    if (_total_deigenstrain_dT)
      paramError("eigenstrain_gradient",
                 "eigenstrain_gradient cannot be specified for materials that provide the "
                 "total_deigenstrain_dT material property");
  }
  if (isParamValid("body_force"))
    _body_force = &getMaterialProperty<RealVectorValue>("body_force");
}

void
InteractionIntegral::initialSetup()
{
  _treat_as_2d = _crack_front_definition->treatAs2D();
}

void
InteractionIntegral::initialize()
{
  std::size_t num_pts;
  if (_treat_as_2d)
    num_pts = 1;
  else
    num_pts = _crack_front_definition->getNumCrackFrontPoints();

  _x.assign(num_pts, 0.0);
  _y.assign(num_pts, 0.0);
  _z.assign(num_pts, 0.0);
  _position.assign(num_pts, 0.0);
  _interaction_integral.assign(num_pts, 0.0);

  for (const auto * fe_var : _fe_vars)
  {
    if (fe_var->feType() != _fe_type)
      mooseError("All coupled displacements must have the same type");
  }
}

Real
InteractionIntegral::computeQpIntegral(const std::size_t crack_front_point_index,
                                       const Real scalar_q,
                                       const RealVectorValue & grad_of_scalar_q)
{
  // In the crack front coordinate system, the crack direction is (1,0,0)
  RealVectorValue crack_direction(1.0, 0.0, 0.0);

  // Calculate (r,theta) position of qp relative to crack front
  _crack_front_definition->calculateRThetaToCrackFront(
      _q_point[_qp], crack_front_point_index, _r, _theta);

  RankTwoTensor aux_stress;
  RankTwoTensor aux_du;

  if (_sif_mode == SifMethod::KI || _sif_mode == SifMethod::KII || _sif_mode == SifMethod::KIII)
    computeAuxFields(aux_stress, aux_du);
  else if (_sif_mode == SifMethod::T)
    computeTFields(aux_stress, aux_du);

  RankTwoTensor grad_disp((*_grad_disp[0])[_qp], (*_grad_disp[1])[_qp], (*_grad_disp[2])[_qp]);

  // Rotate stress, strain, displacement and temperature to crack front coordinate system
  RealVectorValue grad_q_cf =
      _crack_front_definition->rotateToCrackFrontCoords(grad_of_scalar_q, crack_front_point_index);
  RankTwoTensor grad_disp_cf =
      _crack_front_definition->rotateToCrackFrontCoords(grad_disp, crack_front_point_index);
  RankTwoTensor stress_cf =
      _crack_front_definition->rotateToCrackFrontCoords((_stress)[_qp], crack_front_point_index);
  RankTwoTensor strain_cf =
      _crack_front_definition->rotateToCrackFrontCoords((_strain)[_qp], crack_front_point_index);
  RealVectorValue grad_temp_cf =
      _crack_front_definition->rotateToCrackFrontCoords(_grad_temp[_qp], crack_front_point_index);

  RankTwoTensor dq;
  dq(0, 0) = crack_direction(0) * grad_q_cf(0);
  dq(0, 1) = crack_direction(0) * grad_q_cf(1);
  dq(0, 2) = crack_direction(0) * grad_q_cf(2);

  // Calculate interaction integral terms

  // Term1 = stress * x1-derivative of aux disp * dq
  RankTwoTensor tmp1 = dq * stress_cf;
  Real term1 = aux_du.doubleContraction(tmp1);

  // Term2 = aux stress * x1-derivative of disp * dq
  RankTwoTensor tmp2 = dq * aux_stress;
  Real term2 = grad_disp_cf(0, 0) * tmp2(0, 0) + grad_disp_cf(1, 0) * tmp2(0, 1) +
               grad_disp_cf(2, 0) * tmp2(0, 2);

  // Term3 = aux stress * strain * dq_x   (= stress * aux strain * dq_x)
  Real term3 = -dq(0, 0) * aux_stress.doubleContraction(strain_cf);

  // Term4 (thermal strain term) = q * aux_stress * alpha * dtheta_x
  // - the term including the derivative of alpha is not implemented
  Real term4 = 0.0;
  if (_has_temp)
  {
    Real sigma_alpha = aux_stress.doubleContraction((*_total_deigenstrain_dT)[_qp]);
    term4 = scalar_q * sigma_alpha * grad_temp_cf(0);
  }

  Real term4a = 0.0; // Gradient of arbitrary eigenstrain
  if (_eigenstrain_gradient)
  {
    // Thermal strain gradient term in Nakamura and Parks, IJSS, 1992:
    // alpha * dT/dx_k*aux_stress*scalar_q
    // Generalization to the gradient of an arbitrary eigenstrain:
    // d_eigenstrain/dx_k*aux_stress*scalar_q
    const RealVectorValue & crack_dir =
        _crack_front_definition->getCrackDirection(crack_front_point_index);
    RankTwoTensor eigenstrain_grad_in_crack_dir = crack_dir * (*_eigenstrain_gradient)[_qp];
    term4a = scalar_q * aux_stress.doubleContraction(eigenstrain_grad_in_crack_dir);
  }

  Real term5 = 0.0; // Body force
  if (_body_force)
  {
    // Body force term in Nakamura and Parks, IJSS, 1992:
    // b_i*aux_du*crack_dir*scalar_q
    const RealVectorValue & crack_dir =
        _crack_front_definition->getCrackDirection(crack_front_point_index);
    term5 = -scalar_q * (*_body_force)[_qp] * aux_du * crack_dir;
  }

  Real q_avg_seg = 1.0;
  if (!_crack_front_definition->treatAs2D())
  {
    q_avg_seg =
        (_crack_front_definition->getCrackFrontForwardSegmentLength(crack_front_point_index) +
         _crack_front_definition->getCrackFrontBackwardSegmentLength(crack_front_point_index)) /
        2.0;
  }

  Real eq = term1 + term2 + term3 + term4 + term4a + term5;

  return eq / q_avg_seg;
}

void
InteractionIntegral::execute()
{
  // calculate phi and dphi for this element
  const std::size_t dim = _current_elem->dim();
  std::unique_ptr<FEBase> fe(FEBase::build(dim, _fe_type));
  fe->attach_quadrature_rule(const_cast<QBase *>(_qrule));
  _phi_curr_elem = &fe->get_phi();
  _dphi_curr_elem = &fe->get_dphi();
  fe->reinit(_current_elem);

  // calculate q for all nodes in this element
  std::size_t ring_base = (_q_function_type == QMethod::Topology) ? 0 : 1;

  for (auto icfp = beginIndex(_interaction_integral); icfp < _interaction_integral.size(); icfp++)
  {
    _q_curr_elem.clear();
    for (std::size_t i = 0; i < _current_elem->n_nodes(); ++i)
    {
      const Node * this_node = _current_elem->node_ptr(i);
      Real q_this_node;

      if (_q_function_type == QMethod::Geometry)
        q_this_node = _crack_front_definition->DomainIntegralQFunction(
            icfp, _ring_index - ring_base, this_node);
      else // QMethod::Topology
        q_this_node = _crack_front_definition->DomainIntegralTopologicalQFunction(
            icfp, _ring_index - ring_base, this_node);

      _q_curr_elem.push_back(q_this_node);
    }

    for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    {
      Real scalar_q = 0.0;
      RealVectorValue grad_of_scalar_q(0.0, 0.0, 0.0);

      for (std::size_t i = 0; i < _current_elem->n_nodes(); ++i)
      {
        scalar_q += (*_phi_curr_elem)[i][_qp] * _q_curr_elem[i];

        for (std::size_t j = 0; j < _current_elem->dim(); ++j)
          grad_of_scalar_q(j) += (*_dphi_curr_elem)[i][_qp](j) * _q_curr_elem[i];
      }

      _interaction_integral[icfp] +=
          _JxW[_qp] * _coord[_qp] * computeQpIntegral(icfp, scalar_q, grad_of_scalar_q);
    }
  }
}

void
InteractionIntegral::finalize()
{
  gatherSum(_interaction_integral);

  for (auto i = beginIndex(_interaction_integral); i < _interaction_integral.size(); ++i)
  {
    if (_has_symmetry_plane)
      _interaction_integral[i] *= 2.0;

    const auto cfp = _crack_front_definition->getCrackFrontPoint(i);
    _x[i] = (*cfp)(0);
    _y[i] = (*cfp)(1);
    _z[i] = (*cfp)(2);

    if (_position_type == PositionType::Angle)
      _position[i] = _crack_front_definition->getAngleAlongFront(i);
    else
      _position[i] = _crack_front_definition->getDistanceAlongFront(i);

    if (_sif_mode == SifMethod::T && !_treat_as_2d)
      _interaction_integral[i] +=
          _poissons_ratio * _crack_front_definition->getCrackFrontTangentialStrain(i);

    _interaction_integral[i] *= _K_factor;
  }
}

void
InteractionIntegral::threadJoin(const UserObject & y)
{
  const InteractionIntegral & uo = static_cast<const InteractionIntegral &>(y);

  for (auto i = beginIndex(_interaction_integral); i < _interaction_integral.size(); ++i)
    _interaction_integral[i] += uo._interaction_integral[i];
}

void
InteractionIntegral::computeAuxFields(RankTwoTensor & aux_stress, RankTwoTensor & grad_disp)
{
  RealVectorValue k(0.0);
  if (_sif_mode == SifMethod::KI)
    k(0) = 1.0;
  else if (_sif_mode == SifMethod::KII)
    k(1) = 1.0;
  else if (_sif_mode == SifMethod::KIII)
    k(2) = 1.0;

  Real t = _theta;
  Real t2 = _theta / 2.0;
  Real tt2 = 3.0 * _theta / 2.0;
  Real st = std::sin(t);
  Real ct = std::cos(t);
  Real st2 = std::sin(t2);
  Real ct2 = std::cos(t2);
  Real stt2 = std::sin(tt2);
  Real ctt2 = std::cos(tt2);
  Real ct2sq = Utility::pow<2>(ct2);
  Real ct2cu = Utility::pow<3>(ct2);
  Real sqrt2PiR = std::sqrt(2.0 * libMesh::pi * _r);

  // Calculate auxiliary stress tensor
  aux_stress.zero();

  aux_stress(0, 0) =
      1.0 / sqrt2PiR * (k(0) * ct2 * (1.0 - st2 * stt2) - k(1) * st2 * (2.0 + ct2 * ctt2));
  aux_stress(1, 1) = 1.0 / sqrt2PiR * (k(0) * ct2 * (1.0 + st2 * stt2) + k(1) * st2 * ct2 * ctt2);
  aux_stress(0, 1) = 1.0 / sqrt2PiR * (k(0) * ct2 * st2 * ctt2 + k(1) * ct2 * (1.0 - st2 * stt2));
  aux_stress(0, 2) = -1.0 / sqrt2PiR * k(2) * st2;
  aux_stress(1, 2) = 1.0 / sqrt2PiR * k(2) * ct2;
  // plane stress
  // Real s33 = 0;
  // plane strain
  aux_stress(2, 2) = _poissons_ratio * (aux_stress(0, 0) + aux_stress(1, 1));

  aux_stress(1, 0) = aux_stress(0, 1);
  aux_stress(2, 0) = aux_stress(0, 2);
  aux_stress(2, 1) = aux_stress(1, 2);

  // Calculate x1 derivative of auxiliary displacements
  grad_disp.zero();

  grad_disp(0, 0) = k(0) / (4.0 * _shear_modulus * sqrt2PiR) *
                        (ct * ct2 * _kappa + ct * ct2 - 2.0 * ct * ct2cu + st * st2 * _kappa +
                         st * st2 - 6.0 * st * st2 * ct2sq) +
                    k(1) / (4.0 * _shear_modulus * sqrt2PiR) *
                        (ct * st2 * _kappa + ct * st2 + 2.0 * ct * st2 * ct2sq - st * ct2 * _kappa +
                         3.0 * st * ct2 - 6.0 * st * ct2cu);

  grad_disp(0, 1) = k(0) / (4.0 * _shear_modulus * sqrt2PiR) *
                        (ct * st2 * _kappa + ct * st2 - 2.0 * ct * st2 * ct2sq - st * ct2 * _kappa -
                         5.0 * st * ct2 + 6.0 * st * ct2cu) +
                    k(1) / (4.0 * _shear_modulus * sqrt2PiR) *
                        (-ct * ct2 * _kappa + 3.0 * ct * ct2 - 2.0 * ct * ct2cu -
                         st * st2 * _kappa + 3.0 * st * st2 - 6.0 * st * st2 * ct2sq);

  grad_disp(0, 2) = k(2) / (_shear_modulus * sqrt2PiR) * (st2 * ct - ct2 * st);
}

void
InteractionIntegral::computeTFields(RankTwoTensor & aux_stress, RankTwoTensor & grad_disp)
{

  Real t = _theta;
  Real st = std::sin(t);
  Real ct = std::cos(t);
  Real stsq = Utility::pow<2>(st);
  Real ctsq = Utility::pow<2>(ct);
  Real ctcu = Utility::pow<3>(ct);
  Real oneOverPiR = 1.0 / (libMesh::pi * _r);

  aux_stress.zero();
  aux_stress(0, 0) = -oneOverPiR * ctcu;
  aux_stress(0, 1) = -oneOverPiR * st * ctsq;
  aux_stress(1, 0) = -oneOverPiR * st * ctsq;
  aux_stress(1, 1) = -oneOverPiR * ct * stsq;
  aux_stress(2, 2) = -oneOverPiR * _poissons_ratio * (ctcu + ct * stsq);

  grad_disp.zero();
  grad_disp(0, 0) = oneOverPiR / (4.0 * _youngs_modulus) *
                    (ct * (4.0 * Utility::pow<2>(_poissons_ratio) - 3.0 + _poissons_ratio) -
                     std::cos(3.0 * t) * (1.0 + _poissons_ratio));
  grad_disp(0, 1) = -oneOverPiR / (4.0 * _youngs_modulus) *
                    (st * (4.0 * Utility::pow<2>(_poissons_ratio) - 3.0 + _poissons_ratio) +
                     std::sin(3.0 * t) * (1.0 + _poissons_ratio));
}
