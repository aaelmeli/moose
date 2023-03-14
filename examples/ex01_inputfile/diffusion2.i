[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 100
  xmin = 0
  xmax = 10
[]
[Variables]
  [T]
    order=FIRST
    family=LAGRANGE
  []
[]

[AuxVariables]
  [flux_var]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Kernels]
  [diff]
    type = MatDiffusion
    variable = T
    diffusivity= 10
  []
[]



[BCs]
  [leftBC]
    type = DirichletBC
    variable = T
    boundary = left
    value = 0
  []
  [rightBC]
    type = DirichletBC
    variable = T
    boundary = right
    value = 10
  []
[]

[AuxKernels]
  [flux_kernel]
    type = DiffusionFluxAux
    diffusivity = 10
    variable = flux_var
    diffusion_variable = T
    component = x
  []
[]

[Executioner]
  type = Steady
  #solve_type = PJFNK
[]
[Outputs]
  exodus = true
[]






