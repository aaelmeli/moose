//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PINSFVMomentumFriction.h"
#include "NS.h"

registerMooseObject("NavierStokesApp", PINSFVMomentumFriction);

InputParameters
PINSFVMomentumFriction::validParams()
{
  InputParameters params = FVElementalKernel::validParams();
  params.addClassDescription(
      "Computes a friction force term on fluid in porous media in the "
      "Navier Stokes i-th momentum equation in Rhie-Chow (incompressible) contexts.");
  params.addParam<MaterialPropertyName>("Darcy_name",
                                        "Name of the Darcy coefficients material property.");
  params.addParam<MaterialPropertyName>("Forchheimer_name",
                                        "Name of the Forchheimer coefficients material property.");
  params.addParam<MooseFunctorName>(NS::porosity, NS::porosity, "Porosity variable.");
  params.addRequiredParam<MooseFunctorName>("rho", "The density.");
  MooseEnum momentum_component("x=0 y=1 z=2");
  params.addRequiredParam<MooseEnum>(
      "momentum_component",
      momentum_component,
      "The component of the momentum equation that this kernel applies to.");
  return params;
}

PINSFVMomentumFriction::PINSFVMomentumFriction(const InputParameters & params)
  : FVElementalKernel(params),
    _index(getParam<MooseEnum>("momentum_component")),
    _cL(isParamValid("Darcy_name") ? &getFunctor<ADRealVectorValue>("Darcy_name") : nullptr),
    _cQ(isParamValid("Forchheimer_name") ? &getFunctor<ADRealVectorValue>("Forchheimer_name")
                                         : nullptr),
    _use_Darcy_friction_model(isParamValid("Darcy_name")),
    _use_Forchheimer_friction_model(isParamValid("Forchheimer_name")),
    _eps(getFunctor<ADReal>(NS::porosity)),
    _rho(getFunctor<ADReal>("rho"))
{
  if (!_use_Darcy_friction_model && !_use_Forchheimer_friction_model)
    mooseError("At least one friction model needs to be specified.");
}

ADReal
PINSFVMomentumFriction::computeQpResidual()
{
  ADReal friction_term = 0;

  if (_use_Darcy_friction_model)
    friction_term += (*_cL)(_current_elem)(_index)*_rho(_current_elem) * _u_functor(_current_elem) /
                     _eps(_current_elem);
  if (_use_Forchheimer_friction_model)
    friction_term += (*_cQ)(_current_elem)(_index)*_rho(_current_elem) * _u_functor(_current_elem) /
                     _eps(_current_elem);

  return friction_term;
}
