diff=1.1
a=1.1

[GlobalParams]
  advected_interp_method = 'average'
[]

[Mesh]
  [./gen_mesh]
    type = GeneratedMeshGenerator
    dim = 1
    xmin = -0.6
    xmax = 0.6
    nx = 64
  [../]
[]

[Problem]
  kernel_coverage_check = off
[]

[Variables]
  [./v]
    family = MONOMIAL
    order = CONSTANT
    fv = true
  [../]
[]

[FVKernels]
  [./advection]
    type = FVAdvection
    variable = v
    velocity = '${a} 0 0'
  [../]
  [./diffusion]
    type = FVDiffusion
    variable = v
    coeff = coeff
  [../]
  [body_v]
    type = FVBodyForce
    variable = v
    function = 'forcing'
  []
[]

[FVBCs]
  [boundary]
    type = FVFunctionDirichletBC
    boundary = 'left right'
    function = 'exact'
    variable = v
  []
[]

[Materials]
  [diff]
    type = ADGenericConstantFunctorMaterial
    prop_names = 'coeff'
    prop_values = '${diff}'
  []
[]

[Functions]
  [exact]
    type = ParsedFunction
    value = '3*x^2 + 2*x + 1'
  []
  [forcing]
    type = ParsedFunction
    value = '-${diff}*6 + ${a} * (6*x + 2)'
    # value = '-${diff}*6'
  []
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
[]

[Outputs]
  exodus = true
  csv = true
[]

[Postprocessors]
  [./error]
    type = ElementL2Error
    variable = v
    function = exact
    outputs = 'console csv'
    execute_on = 'timestep_end'
  [../]
  [h]
    type = AverageElementSize
    outputs = 'console csv'
    execute_on = 'timestep_end'
  []
[]
