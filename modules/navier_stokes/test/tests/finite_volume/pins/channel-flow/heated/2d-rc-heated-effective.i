mu=1
rho=1
k=1e-3
cp=1
u_inlet=1
T_inlet=200
advected_interp_method='average'
velocity_interp_method='rc'

[Mesh]
  [gen]
    type = GeneratedMeshGenerator
    dim = 2
    xmin = 0
    xmax = 10
    ymin = 0
    ymax = 1
    nx = 100
    ny = 20
  []
[]

[Variables]
  inactive = 'temp_solid'
  [u]
    type = PINSFVSuperficialVelocityVariable
    initial_condition = ${u_inlet}
  []
  [v]
    type = PINSFVSuperficialVelocityVariable
    initial_condition = 1e-6
  []
  [pressure]
    type = INSFVPressureVariable
  []
  [temperature]
    type = INSFVEnergyVariable
  []
  [temp_solid]
    family = 'MONOMIAL'
    order = 'CONSTANT'
    fv = true
  []
[]

[AuxVariables]
  [temp_solid]
    family = 'MONOMIAL'
    order = 'CONSTANT'
    fv = true
    initial_condition = 100
  []
  [porosity]
    family = MONOMIAL
    order = CONSTANT
    fv = true
    initial_condition = 0.5
  []
[]

[FVKernels]
  inactive = 'solid_energy_diffusion solid_energy_convection'
  [mass]
    type = PINSFVMassAdvection
    variable = pressure
    advected_interp_method = ${advected_interp_method}
    velocity_interp_method = ${velocity_interp_method}
    vel = 'velocity'
    pressure = pressure
    u = u
    v = v
    mu = ${mu}
    rho = ${rho}
    porosity = porosity
  []

  [u_advection]
    type = PINSFVMomentumAdvection
    variable = u
    advected_quantity = 'rhou'
    vel = 'velocity'
    advected_interp_method = ${advected_interp_method}
    velocity_interp_method = ${velocity_interp_method}
    pressure = pressure
    u = u
    v = v
    mu = ${mu}
    rho = ${rho}
    porosity = porosity
  []
  [u_viscosity]
    type = PINSFVMomentumDiffusion
    variable = u
    mu = ${mu}
    porosity = porosity
  []
  [u_pressure]
    type = PINSFVMomentumPressure
    variable = u
    momentum_component = 'x'
    pressure = pressure
    porosity = porosity
  []

  [v_advection]
    type = PINSFVMomentumAdvection
    variable = v
    advected_quantity = 'rhov'
    vel = 'velocity'
    advected_interp_method = ${advected_interp_method}
    velocity_interp_method = ${velocity_interp_method}
    pressure = pressure
    u = u
    v = v
    mu = ${mu}
    rho = ${rho}
    porosity = porosity
  []
  [v_viscosity]
    type = PINSFVMomentumDiffusion
    variable = v
    mu = ${mu}
    porosity = porosity
  []
  [v_pressure]
    type = PINSFVMomentumPressure
    variable = v
    momentum_component = 'y'
    pressure = pressure
    porosity = porosity
  []

  [energy_advection]
    type = PINSFVEnergyAdvection
    variable = temperature
    vel = 'velocity'
    velocity_interp_method = ${velocity_interp_method}
    advected_interp_method = ${advected_interp_method}
    pressure = pressure
    u = u
    v = v
    mu = ${mu}
    rho = ${rho}
    porosity = porosity
  []
  [energy_diffusion]
    type = PINSFVEnergyEffectiveDiffusion
    kappa = 'kappa'
    variable = temperature
  []
  [energy_convection]
    type = PINSFVEnergyAmbientConvection
    variable = temperature
    is_solid = false
    temp_fluid = temperature
    temp_solid = temp_solid
    h_solid_fluid = 'h_cv'
  []

  [solid_energy_diffusion]
    type = FVDiffusion
    coeff = ${k}
    variable = temp_solid
  []
  [solid_energy_convection]
    type = PINSFVEnergyAmbientConvection
    variable = temp_solid
    is_solid = true
    temp_fluid = temperature
    temp_solid = temp_solid
    h_solid_fluid = 'h_cv'
  []
[]

[FVBCs]
  inactive = 'heated-side'
  [inlet-u]
    type = INSFVInletVelocityBC
    boundary = 'left'
    variable = u
    function = ${u_inlet}
  []
  [inlet-v]
    type = INSFVInletVelocityBC
    boundary = 'left'
    variable = v
    function = 0
  []
  [inlet-T]
    type = FVNeumannBC
    variable = temperature
    value = ${fparse u_inlet * rho * cp * T_inlet}
    boundary = 'left'
  []

  [no-slip-u]
    type = INSFVNoSlipWallBC
    boundary = 'top'
    variable = u
    function = 0
  []
  [no-slip-v]
    type = INSFVNoSlipWallBC
    boundary = 'top'
    variable = v
    function = 0
  []
  [heated-side]
    type = FVDirichletBC
    boundary = 'top'
    variable = 'temp_solid'
    value = 150
  []

  [symmetry-u]
    type = PINSFVSymmetryVelocityBC
    boundary = 'bottom'
    variable = u
    u = u
    v = v
    mu = ${mu}
    momentum_component = 'x'
    porosity = porosity
  []
  [symmetry-v]
    type = PINSFVSymmetryVelocityBC
    boundary = 'bottom'
    variable = v
    u = u
    v = v
    mu = ${mu}
    momentum_component = 'y'
    porosity = porosity
  []
  [symmetry-p]
    type = INSFVSymmetryPressureBC
    boundary = 'bottom'
    variable = pressure
  []

  [outlet-p]
    type = INSFVOutletPressureBC
    boundary = 'right'
    variable = pressure
    function = 0.1
  []
[]

[Materials]
  [constants]
    type = ADGenericConstantMaterial
    prop_names = 'h_cv'
    prop_values = '1'
  []
  [functor_constants]
    type = ADGenericConstantFunctorMaterial
    prop_names = 'cp'
    prop_values = '${cp}'
  []
  [kappa]
    type = ADGenericConstantVectorMaterial
    prop_names = 'kappa'
    prop_values = '1e-3 1e-2 1e-1'
  []
  [ins_fv]
    type = INSFVMaterial
    u = 'u'
    v = 'v'
    pressure = 'pressure'
    rho = ${rho}
    temperature = 'temperature'
  []
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type -ksp_gmres_restart -sub_pc_type -sub_pc_factor_shift_type'
  petsc_options_value = 'asm      100                lu           NONZERO'
  line_search = 'none'
  nl_rel_tol = 1e-12
[]

# Some basic Postprocessors to examine the solution
[Postprocessors]
  [inlet-p]
    type = SideAverageValue
    variable = pressure
    boundary = 'left'
  []
  [outlet-u]
    type = SideAverageValue
    variable = u
    boundary = 'right'
  []
  [outlet-temp]
    type = SideAverageValue
    variable = temperature
    boundary = 'right'
  []
  [solid-temp]
    type = ElementAverageValue
    variable = temp_solid
  []
[]

[Outputs]
  exodus = true
  csv = false
[]
