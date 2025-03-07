//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "INSFVMixingLengthScalarDiffusion.h"

registerMooseObject("NavierStokesApp", INSFVMixingLengthScalarDiffusion);

InputParameters
INSFVMixingLengthScalarDiffusion::validParams()
{
  InputParameters params = FVFluxKernel::validParams();
  params.addClassDescription("Computes the turbulent diffusive flux that appears in "
                             "Reynolds-averaged fluid conservation equations.");
  params.addRequiredCoupledVar("u", "The velocity in the x direction.");
  params.addCoupledVar("v", "The velocity in the y direction.");
  params.addCoupledVar("w", "The velocity in the z direction.");
  params.addRequiredCoupledVar("mixing_length", "The turbulent mixing length.");
  params.addRequiredParam<Real>(
      "schmidt_number",
      "The turbulent Schmidt number (or turbulent Prandtl number if the passive scalar is energy) "
      "that relates the turbulent scalar diffusivity to the turbulent momentum diffusivity.");
  params.set<unsigned short>("ghost_layers") = 2;
  return params;
}

INSFVMixingLengthScalarDiffusion::INSFVMixingLengthScalarDiffusion(const InputParameters & params)
  : FVFluxKernel(params),
    _dim(_subproblem.mesh().dimension()),
    _u_var(dynamic_cast<const INSFVVelocityVariable *>(getFieldVar("u", 0))),
    _v_var(isParamValid("v") ? dynamic_cast<const INSFVVelocityVariable *>(getFieldVar("v", 0))
                             : nullptr),
    _w_var(isParamValid("w") ? dynamic_cast<const INSFVVelocityVariable *>(getFieldVar("w", 0))
                             : nullptr),
    _mixing_len(*getVarHelper<MooseVariableFV<Real>>("mixing_length", 0)),
    _schmidt_number(getParam<Real>("schmidt_number"))
{
#ifndef MOOSE_GLOBAL_AD_INDEXING
  mooseError("INSFV is not supported by local AD indexing. In order to use INSFV, please run the "
             "configure script in the root MOOSE directory with the configure option "
             "'--with-ad-indexing-type=global'");
#endif

  if (!_u_var)
    paramError("u", "the u velocity must be an INSFVVelocityVariable.");

  if (_dim >= 2 && !_v_var)
    paramError("v",
               "In two or more dimensions, the v velocity must be supplied and it must be an "
               "INSFVVelocityVariable.");

  if (_dim >= 3 && !_w_var)
    paramError("w",
               "In three-dimensions, the w velocity must be supplied and it must be an "
               "INSFVVelocityVariable.");
}

ADReal
INSFVMixingLengthScalarDiffusion::computeQpResidual()
{
#ifdef MOOSE_GLOBAL_AD_INDEXING
  constexpr Real offset = 1e-15; // prevents explosion of sqrt(x) derivative to infinity

  const auto & grad_u = _u_var->adGradSln(*_face_info);
  ADReal symmetric_strain_tensor_norm = 2.0 * Utility::pow<2>(grad_u(0));
  if (_dim >= 2)
  {
    auto grad_v = _v_var->adGradSln(*_face_info);
    symmetric_strain_tensor_norm +=
        2.0 * Utility::pow<2>(grad_v(1)) + Utility::pow<2>(grad_v(0) + grad_u(1));
    if (_dim >= 3)
    {
      auto grad_w = _w_var->adGradSln(*_face_info);
      symmetric_strain_tensor_norm += 2.0 * Utility::pow<2>(grad_w(2)) +
                                      Utility::pow<2>(grad_u(2) + grad_w(0)) +
                                      Utility::pow<2>(grad_v(2) + grad_w(1));
    }
  }

  symmetric_strain_tensor_norm = std::sqrt(symmetric_strain_tensor_norm + offset);

  // Interpolate the mixing length to the face
  ADReal mixing_len = _mixing_len(std::make_tuple(
      _face_info, Moose::FV::LimiterType::CentralDifference, true, faceArgSubdomains(_face_info)));

  // Compute the eddy diffusivity for momentum
  ADReal eddy_diff = symmetric_strain_tensor_norm * mixing_len * mixing_len;

  // Use the turbulent Schmidt/Prandtl number to get the eddy diffusivity for
  // the scalar variable
  eddy_diff /= _schmidt_number;

  // Compute the diffusive flux of the scalar variable
  auto dudn = gradUDotNormal();
  return -1 * eddy_diff * dudn;

#else
  return 0;

#endif
}
