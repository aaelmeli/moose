//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FunctorMaterial.h"

template <typename>
class MooseVariableFV;
template <typename>
class FunctorMaterialProperty;

/**
 * A material that couples a variable
 */
class VarFunctorMaterial : public FunctorMaterial
{
public:
  static InputParameters validParams();

  VarFunctorMaterial(const InputParameters & parameters);

protected:
  const Moose::Functor<ADReal> & _var;
  FunctorMaterialProperty<ADReal> & _functor_prop;
};
