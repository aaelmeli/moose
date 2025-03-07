//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "PolygonConcentricCircleMeshGeneratorBase.h"
#include "libmesh/mesh_smoother_laplace.h"

#include <cmath>

InputParameters
PolygonConcentricCircleMeshGeneratorBase::validParams()
{
  InputParameters params = PolygonMeshGeneratorBase::validParams();
  params.addRequiredRangeCheckedParam<std::vector<unsigned int>>(
      "num_sectors_per_side",
      "num_sectors_per_side>0",
      "Number of azimuthal sectors per polygon side (rotating counterclockwise from top right "
      "face).");
  params.addRangeCheckedParam<unsigned int>(
      "background_intervals",
      1,
      "background_intervals>0",
      "Number of radial meshing intervals in background region (area between rings and ducts).");
  params.addParam<std::vector<subdomain_id_type>>(
      "background_block_ids", "Optional customized block id for the background block.");
  params.addParam<std::vector<SubdomainName>>(
      "background_block_names", "Optional customized block names for the background block.");
  params.addParam<std::vector<Real>>(
      "duct_sizes", "Distance(s) from polygon center to duct(s) inner boundaries.");
  MooseEnum duct_sizes_style("apothem radius", "radius");
  params.addParam<MooseEnum>(
      "duct_sizes_style",
      duct_sizes_style,
      "Style in which polygon center to duct inner boundary distance is "
      "given (apothem = center to face, radius = center to vertex). Options: " +
          duct_sizes_style.getRawNames());
  params.addRangeCheckedParam<std::vector<unsigned int>>(
      "duct_intervals", "duct_intervals>0", "Number of meshing intervals in each enclosing duct.");
  params.addParam<std::vector<subdomain_id_type>>(
      "duct_block_ids", "Optional customized block ids for each duct geometry block.");
  params.addParam<std::vector<SubdomainName>>(
      "duct_block_names", "Optional customized block names for each duct geometry block.");
  params.addRangeCheckedParam<std::vector<Real>>(
      "ring_radii", "ring_radii>0", "Radii of major concentric circles (rings).");
  params.addRangeCheckedParam<std::vector<unsigned int>>(
      "ring_intervals",
      "ring_intervals>0",
      "Number of radial mesh intervals within each major concentric circle.");
  params.addParam<std::vector<subdomain_id_type>>(
      "ring_block_ids", "Optional customized block ids for each ring geometry block.");
  params.addParam<std::vector<SubdomainName>>(
      "ring_block_names", "Optional customized block names for each ring geometry block.");
  params.addParam<bool>("preserve_volumes",
                        true,
                        "Volume of concentric circles can be preserved using this function.");
  params.addParam<subdomain_id_type>("block_id_shift", 0, "Integer used to shift block IDs.");
  params.addParam<boundary_id_type>(
      "interface_boundary_id_shift", 0, "Integer used to shift interface boundary IDs.");
  params.addRangeCheckedParam<boundary_id_type>("external_boundary_id",
                                                "external_boundary_id>0",
                                                "Optional customized external boundary id.");
  params.addParam<std::string>("external_boundary_name",
                               "Optional customized external boundary name.");
  params.addParam<std::vector<std::string>>(
      "interface_boundary_names",
      "Optional customized boundary names for the internal interfaces between block.");
  params.addParam<bool>("uniform_mesh_on_sides",
                        false,
                        "Whether the side elements are reorganized to have a uniform size.");
  params.addParam<bool>(
      "quad_center_elements", false, "Whether the center elements are quad or triangular.");
  params.addParam<unsigned int>("smoothing_max_it",
                                0,
                                "Number of Laplacian smoothing iterations. This number is "
                                "disregarded when duct_sizes is present.");
  params.addParamNamesToGroup(
      "background_block_ids background_block_names duct_block_ids duct_block_names ring_block_ids "
      "ring_block_names external_boundary_id external_boundary_name interface_boundary_names "
      "block_id_shift interface_boundary_id_shift",
      "Customized Subdomain/Boundary");
  params.addParamNamesToGroup("num_sectors_per_side background_intervals duct_intervals "
                              "ring_intervals uniform_mesh_on_sides",
                              "Mesh Density");
  params.addClassDescription("This PolygonConcentricCircleMeshGeneratorBase object is a base class "
                             "to be inherited for polygon mesh generators.");

  return params;
}

PolygonConcentricCircleMeshGeneratorBase::PolygonConcentricCircleMeshGeneratorBase(
    const InputParameters & parameters)
  : PolygonMeshGeneratorBase(parameters),
    _num_sides(isParamValid("num_sides") ? getParam<unsigned int>("num_sides")
                                         : (unsigned int)HEXAGON_NUM_SIDES),
    _ring_radii(isParamValid("ring_radii") ? getParam<std::vector<Real>>("ring_radii")
                                           : std::vector<Real>()),
    _ring_intervals(isParamValid("ring_intervals")
                        ? getParam<std::vector<unsigned int>>("ring_intervals")
                        : std::vector<unsigned int>()),
    _ring_block_ids(isParamValid("ring_block_ids")
                        ? getParam<std::vector<subdomain_id_type>>("ring_block_ids")
                        : std::vector<subdomain_id_type>()),
    _ring_block_names(isParamValid("ring_block_names")
                          ? getParam<std::vector<SubdomainName>>("ring_block_names")
                          : std::vector<SubdomainName>()),
    _duct_sizes_style(getParam<MooseEnum>("duct_sizes_style").template getEnum<DuctStyle>()),
    _duct_sizes(isParamValid("duct_sizes") ? getParam<std::vector<Real>>("duct_sizes")
                                           : std::vector<Real>()),
    _duct_intervals(isParamValid("duct_intervals")
                        ? getParam<std::vector<unsigned int>>("duct_intervals")
                        : std::vector<unsigned int>()),
    _duct_block_ids(isParamValid("duct_block_ids")
                        ? getParam<std::vector<subdomain_id_type>>("duct_block_ids")
                        : std::vector<subdomain_id_type>()),
    _duct_block_names(isParamValid("duct_block_names")
                          ? getParam<std::vector<SubdomainName>>("duct_block_names")
                          : std::vector<SubdomainName>()),
    _has_rings(isParamValid("ring_radii")),
    _has_ducts(isParamValid("duct_sizes")),
    _polygon_size_style(
        isParamValid("polygon_size_style")
            ? getParam<MooseEnum>("polygon_size_style").template getEnum<PolygonStyle>()
            : getParam<MooseEnum>("hexagon_size_style").template getEnum<PolygonStyle>()),
    _polygon_size(isParamValid("polygon_size") ? getParam<Real>("polygon_size")
                                               : getParam<Real>("hexagon_size")),
    _num_sectors_per_side(getParam<std::vector<unsigned int>>("num_sectors_per_side")),
    _background_intervals(getParam<unsigned int>("background_intervals")),
    // background is usually a single block; however, when there are no rings, background has two
    // blocks.
    _background_block_ids(isParamValid("background_block_ids")
                              ? getParam<std::vector<subdomain_id_type>>("background_block_ids")
                              : std::vector<subdomain_id_type>()),
    _background_block_names(isParamValid("background_block_names")
                                ? getParam<std::vector<SubdomainName>>("background_block_names")
                                : std::vector<SubdomainName>()),
    _preserve_volumes(getParam<bool>("preserve_volumes")),
    _block_id_shift(getParam<subdomain_id_type>("block_id_shift")),
    _interface_boundary_id_shift(getParam<boundary_id_type>("interface_boundary_id_shift")),
    _external_boundary_id(isParamValid("external_boundary_id")
                              ? getParam<boundary_id_type>("external_boundary_id")
                              : 0),
    _external_boundary_name(isParamValid("external_boundary_name")
                                ? getParam<std::string>("external_boundary_name")
                                : std::string()),
    _interface_boundary_names(isParamValid("interface_boundary_names")
                                  ? getParam<std::vector<std::string>>("interface_boundary_names")
                                  : std::vector<std::string>()),
    _uniform_mesh_on_sides(getParam<bool>("uniform_mesh_on_sides")),
    _quad_center_elements(getParam<bool>("quad_center_elements")),
    _smoothing_max_it(getParam<unsigned int>("smoothing_max_it")),
    _sides_to_adapt(isParamValid("sides_to_adapt")
                        ? getParam<std::vector<unsigned int>>("sides_to_adapt")
                        : std::vector<unsigned int>()),
    _pitch_meta(declareMeshProperty<Real>("pitch_meta", 0.0)),
    _background_intervals_meta(declareMeshProperty<unsigned int>("background_intervals_meta", 0)),
    _node_id_background_meta(declareMeshProperty<dof_id_type>("node_id_background_meta", 0)),
    _pattern_pitch_meta(declareMeshProperty<Real>("pattern_pitch_meta", 0.0)),
    _azimuthal_angle_meta(
        declareMeshProperty<std::vector<Real>>("azimuthal_angle_meta", std::vector<Real>())),
    _is_control_drum_meta(declareMeshProperty<bool>("is_control_drum_meta", false))
{
  // This error message is only reserved for future derived classes. Neither of the current derived
  // classes will trigger this error.
  if (!_sides_to_adapt.empty() && _num_sides != HEXAGON_NUM_SIDES)
    paramError("sides_to_adapt", "If provided, the generated mesh must be a hexagon.");
  _pitch = 2.0 * (_polygon_size_style == PolygonStyle::apothem
                      ? _polygon_size
                      : _polygon_size * std::cos(M_PI / Real(_num_sides)));
  _pitch_meta = _pitch;
  if (_interface_boundary_names.size() > 0 &&
      _interface_boundary_names.size() != _duct_sizes.size() + _ring_radii.size())
    paramError("interface_boundary_names",
               "If provided, the length of this parameter must be identical to the total number of "
               "interfaces.");
  if ((_has_rings || _background_intervals == 1) && _background_block_ids.size() > 1)
    paramError("background_block_ids",
               "This parameter must be either unset or have a unity length when ring_radii is "
               "provided or background_intervals is unity.");
  if ((_has_rings || _background_intervals == 1) && _background_block_names.size() > 1)
    paramError("background_block_names",
               "This parameter must be either unset or have a unity length when ring_radii is "
               "provided or background_intervals is unity.");
  if ((!_has_rings && _background_intervals > 1) &&
      (!_background_block_ids.empty() && _background_block_ids.size() != 2))
    paramError("background_block_ids",
               "This parameter must be either unset or have a length of two when ring_radii is not "
               "provided and background_intervals is not unity.");
  if ((!_has_rings && _background_intervals > 1) &&
      (!_background_block_names.empty() && _background_block_names.size() != 2))
    paramError("background_block_names",
               "This parameter must be either unset or have a length of two when ring_radii is not "
               "provided and background_intervals is not unity.");
  if (_num_sectors_per_side.size() != _num_sides)
    paramError("num_sectors_per_side",
               "This parameter must have a length that is consistent with num_sides.");
  for (auto it = _num_sectors_per_side.begin(); it != _num_sectors_per_side.end(); ++it)
    if (*it % 2 == 1)
      paramError("num_sectors_per_side", "This parameter must be even.");
  _num_sectors_per_side_meta =
      declareMeshProperty("num_sectors_per_side_meta", _num_sectors_per_side);
  if (_has_rings)
  {
    if (_ring_radii.size() != _ring_intervals.size())
      paramError("ring_radii", "This parameter and ring_intervals must have the same length.");
    if (!_ring_block_ids.empty() &&
        _ring_block_ids.size() !=
            (_ring_intervals.size() + (unsigned int)(_ring_intervals.front() != 1)))
      paramError("ring_block_ids",
                 "This parameter must have the appropriate size if it is provided.");
    if (!_ring_block_names.empty() &&
        _ring_block_names.size() !=
            (_ring_intervals.size() + (unsigned int)(_ring_intervals.front() != 1)))
      paramError("ring_block_names", "This parameter must have the appropriate size if it is set.");
    for (unsigned int i = 1; i < _ring_intervals.size(); i++)
      if (_ring_radii[i] <= _ring_radii[i - 1])
        paramError("ring_radii", "This parameter must be strictly ascending.");
  }
  if (_has_ducts)
  {
    if (_duct_sizes.size() != _duct_intervals.size())
      paramError("duct_sizes", "This parameter and duct_intervals must have the same length.");
    if (!_duct_block_ids.empty() && _duct_block_ids.size() != _duct_intervals.size())
      paramError("duct_block_ids",
                 "This parameter must have the same length as duct_intervals if set.");
    if (!_duct_block_names.empty() && _duct_block_names.size() != _duct_intervals.size())
      paramError("duct_block_names",
                 "This parameter must have the same length as duct_intervals if set.");
    if (_duct_sizes_style == DuctStyle::apothem)
      for (unsigned int i = 0; i < _duct_sizes.size(); i++)
        _duct_sizes[i] /= std::cos(M_PI / Real(_num_sides));
    for (unsigned int i = 1; i < _duct_sizes.size(); i++)
      if (_duct_sizes[i] <= _duct_sizes[i - 1])
        paramError("duct_sizes", "This parameter must be strictly ascending.");
    if (_duct_sizes.front() <=
        (_has_rings ? _ring_radii.back() / std::cos(M_PI / Real(_num_sides)) : 0.0))
      paramError("duct_sizes",
                 "This parameter must be positive and ensures no overlapping with rings.");
    if (_duct_sizes.back() >= _pitch / 2.0 / std::cos(M_PI / Real(_num_sides)))
      paramError("duct_sizes",
                 "This parameter must ensure that ducts are smaller than the polygon size.");
    if (*std::min_element(_duct_intervals.begin(), _duct_intervals.end()) <= 0)
      paramError("duct_intervals", "Elements of this parameter must be positive.");
  }
}

std::unique_ptr<MeshBase>
PolygonConcentricCircleMeshGeneratorBase::generate()
{
  unsigned int mesh_input_counter = 0;
  // azimuthal array used for radius radius_correction
  std::vector<Real> azimuthal_list;
  // loop over all sides of the polygon to collect azimuthal angles of all nodes
  for (unsigned int mesh_index = 0; mesh_index < _num_sides; mesh_index++)
  {
    // When adaptive boundaries exist (only possible for hexagon meshes thru
    // `HexagonConcentricCircleAdaptiveBoundaryMeshGenerator`), nodes' azimuthal angle cannot be
    // arithmetically obtained. Instead, `azimuthalAnglesCollector() is used to get this
    // information from the mesh directly.`
    if (std::find(_sides_to_adapt.begin(), _sides_to_adapt.end(), mesh_index) !=
        _sides_to_adapt.end())
    {
      // The following lines only work for hexagon; and only a hexagon needs such functionality.
      Real lower_azi = (Real)mesh_index * 60.0 - 150.0;
      Real upper_azi = (Real)((mesh_index + 1) % 6) * 60.0 - 150.0;
      _azimuthal_angles_array.push_back(azimuthalAnglesCollector(
          dynamic_pointer_cast<ReplicatedMesh>(*_input_ptrs[mesh_input_counter]),
          lower_azi,
          upper_azi));
      // loop over the _azimuthal_angles_array just collected to convert tangent to azimuthal
      // angles.
      for (unsigned int i = 1; i < _azimuthal_angles_array.back().size(); i++)
      {
        azimuthal_list.push_back(
            (Real)mesh_index * 60.0 - 150.0 +
            std::atan((_azimuthal_angles_array.back()[i - 1] - 1.0) / std::sqrt(3.0)) * 180.0 /
                M_PI);
      }
      mesh_input_counter++;
    }
    else
    {
      _azimuthal_angles_array.push_back(std::vector<Real>());
      for (unsigned int i = 0; i < _num_sectors_per_side[mesh_index]; i++)
      {
        azimuthal_list.push_back(
            std::atan(std::tan(M_PI / _num_sides) *
                      (2.0 * (Real)i / (Real)_num_sectors_per_side[mesh_index] - 1.0)) *
                180.0 / M_PI +
            (Real)mesh_index * (360.0 / (Real)_num_sides) - (180.0 - 180.0 / (Real)_num_sides));
      }
    }
  }
  std::vector<Real> ring_radii_corr;
  if (_has_rings)
  {
    if (_preserve_volumes)
    {
      Real corr_factor = radiusCorrectionFactor(azimuthal_list);
      for (unsigned int i = 0; i < _ring_radii.size(); i++)
        ring_radii_corr.push_back(_ring_radii[i] * corr_factor);
    }
    else
      ring_radii_corr = _ring_radii;
    if (ring_radii_corr.back() >= _pitch / 2.0)
      paramError("ring_radii",
                 "Elements of this parameter must be smaller than polygon apothem (after volume "
                 "preserve correction if applicable).");
  }
  auto mesh = buildReplicatedMesh(2);
  // build the first slice of the polygon.
  auto mesh0 = buildSimpleSlice(dynamic_pointer_cast<ReplicatedMesh>(mesh),
                                ring_radii_corr,
                                _ring_intervals,
                                _duct_sizes,
                                _duct_intervals,
                                _has_rings,
                                _has_ducts,
                                _pitch,
                                _num_sectors_per_side[0],
                                _background_intervals,
                                &_node_id_background_meta,
                                _num_sides,
                                1,
                                _azimuthal_angles_array[0],
                                _block_id_shift,
                                _quad_center_elements,
                                _interface_boundary_id_shift);
  // This loop builds add-on slices and stitches them to the first slice
  for (unsigned int mesh_index = 1; mesh_index < _num_sides; mesh_index++)
  {
    auto mesh_tmp0 = buildReplicatedMesh(2);
    auto mesh_tmp = buildSimpleSlice(dynamic_pointer_cast<ReplicatedMesh>(mesh_tmp0),
                                     ring_radii_corr,
                                     _ring_intervals,
                                     _duct_sizes,
                                     _duct_intervals,
                                     _has_rings,
                                     _has_ducts,
                                     _pitch,
                                     _num_sectors_per_side[mesh_index],
                                     _background_intervals,
                                     &_node_id_background_meta,
                                     _num_sides,
                                     mesh_index + 1,
                                     _azimuthal_angles_array[mesh_index],
                                     _block_id_shift,
                                     _quad_center_elements,
                                     _interface_boundary_id_shift);

    ReplicatedMesh other_mesh(*mesh_tmp);
    MeshTools::Modification::rotate(other_mesh, 360.0 / _num_sides * mesh_index, 0, 0);
    mesh0->prepare_for_use();
    other_mesh.prepare_for_use();
    mesh0->stitch_meshes(other_mesh, SLICE_BEGIN, SLICE_END, TOLERANCE, true);
    other_mesh.clear();
  }

  // An extra step to stich the first and last slices together
  mesh0->stitch_surfaces(SLICE_BEGIN, SLICE_END, TOLERANCE, true);

  if (!_has_rings && !_has_ducts && _background_intervals == 1)
    MooseMesh::changeBoundaryId(*mesh0, 1, OUTER_SIDESET_ID, false);

  if (!_is_general_polygon)
  {
    auto mesh0_dup = mesh0->clone();
    _azimuthal_angle_meta = azimuthalAnglesCollector(
        dynamic_pointer_cast<ReplicatedMesh>(mesh0_dup), -180.0, 180.0, ANGLE_DEGREE);
    _pattern_pitch_meta = _pitch;
  }

  // Move nodes on the external boundary for force uniform spacing.
  if (_uniform_mesh_on_sides)
  {
    const Real angle_tol = 1.0e-5;
    std::vector<std::pair<Real, unsigned int>> node_azi_list;
    MeshTools::Modification::rotate(*mesh0, -(270.0 - 360.0 / (Real)_num_sides), 0.0, 0.0);
    std::vector<std::tuple<dof_id_type, unsigned short int, boundary_id_type>> side_list =
        mesh0->get_boundary_info().build_side_list();
    mesh0->get_boundary_info().build_node_list_from_side_list();
    std::vector<std::tuple<dof_id_type, boundary_id_type>> node_list =
        mesh0->get_boundary_info().build_node_list();

    for (unsigned int i = 0; i < node_list.size(); ++i)
    {
      if (std::get<1>(node_list[i]) == OUTER_SIDESET_ID)
      {
        node_azi_list.push_back(
            std::make_pair(atan2((*mesh0->node_ptr(std::get<0>(node_list[i])))(1),
                                 (*mesh0->node_ptr(std::get<0>(node_list[i])))(0)) *
                               180.0 / M_PI,
                           std::get<0>(node_list[i])));
        if (std::abs(node_azi_list.back().first + 180.0) <= angle_tol)
          node_azi_list.back().first = 180.0;
      }
    }
    std::sort(node_azi_list.begin(), node_azi_list.end());
    for (unsigned int i = 0; i < _num_sides; i++)
    {
      for (unsigned int j = 1; j <= _num_sectors_per_side[i]; j++)
      {
        Real azi_corr_tmp = atan2((Real)j * 2.0 / (Real)_num_sectors_per_side[i] - 1.0,
                                  1.0 / std::tan(M_PI / (Real)_num_sides));
        Real x_tmp = _pitch / 2.0;
        Real y_tmp = x_tmp * std::tan(azi_corr_tmp);
        nodeCoordRotate(&x_tmp,
                        &y_tmp,
                        (Real)i * 360.0 / (Real)_num_sides - (180.0 - 180.0 / (Real)_num_sides));
        Point p_tmp = Point(x_tmp, y_tmp, 0.0);
        mesh0->add_point(
            p_tmp,
            node_azi_list[std::accumulate(
                              _num_sectors_per_side.begin(), _num_sectors_per_side.begin() + i, 0) +
                          j - 1]
                .second);
      }
    }
    MeshTools::Modification::rotate(*mesh0, (270.0 - 360.0 / (Real)_num_sides), 0.0, 0.0);
  }

  _background_intervals_meta = _background_intervals;

  if (!_has_ducts && _sides_to_adapt.empty())
  {
    LaplaceMeshSmoother lms(*mesh0);
    lms.smooth(_smoothing_max_it);
  }

  // Set up customized Block Names and/or IDs
  unsigned int block_it = 0;
  unsigned ring_block_num = 0;
  unsigned background_block_num = 1;
  std::vector<subdomain_id_type> block_ids_old;
  std::vector<subdomain_id_type> block_ids_new;
  std::vector<SubdomainName> block_names;
  if (_has_rings)
  {
    if (_ring_intervals.front() == 1)
      ring_block_num = _ring_intervals.size();
    else
    {
      ring_block_num = _ring_intervals.size() + 1;
      block_ids_old.push_back(_block_id_shift + 1);
      block_ids_new.push_back(_ring_block_ids.empty() ? block_ids_old.back()
                                                      : _ring_block_ids.front());
      block_names.push_back(_ring_block_names.empty()
                                ? (SubdomainName)std::to_string(block_ids_new.back())
                                : _ring_block_names.front());
      block_it++;
    }
    for (unsigned int i = ring_block_num - _ring_intervals.size(); i < ring_block_num; i++)
    {
      block_ids_old.push_back(_block_id_shift + 1 + i);
      block_ids_new.push_back(_ring_block_ids.empty() ? block_ids_old.back() : _ring_block_ids[i]);
      block_names.push_back(_ring_block_names.empty()
                                ? (SubdomainName)std::to_string(block_ids_new.back())
                                : _ring_block_names[i]);
      block_it++;
    }
  }
  else
  {
    if (_background_intervals > 1)
    {
      background_block_num++;
      block_ids_old.push_back(_block_id_shift + 1 + block_it);
      block_ids_new.push_back(_background_block_ids.empty() ? block_ids_old.back()
                                                            : _background_block_ids.front());
      block_names.push_back(_background_block_names.empty()
                                ? (SubdomainName)std::to_string(block_ids_new.back())
                                : _background_block_names.front());
      block_it++;
    }
  }
  block_ids_old.push_back(_block_id_shift + 1 + block_it);
  block_ids_new.push_back(_background_block_ids.empty() ? block_ids_old.back()
                                                        : _background_block_ids.back());
  block_names.push_back(_background_block_names.empty()
                            ? (SubdomainName)std::to_string(block_ids_new.back())
                            : _background_block_names.back());
  block_it++;
  if (_has_ducts)
  {
    for (unsigned int i = 0; i < _duct_intervals.size(); i++)
    {
      block_ids_old.push_back(_block_id_shift + 1 + block_it);
      block_ids_new.push_back(_duct_block_ids.empty() ? block_ids_old.back() : _duct_block_ids[i]);
      block_names.push_back(_duct_block_names.empty()
                                ? (SubdomainName)std::to_string(block_ids_new.back())
                                : _duct_block_names[i]);
      block_it++;
    }
  }

  for (auto it = block_names.begin(); it != block_names.end() - 1; it++)
  {
    auto it_tmp = std::find(block_names.begin(), it + 1, *(it + 1));
    if (it_tmp != it + 1 && block_ids_new[std::distance(block_names.begin(), it + 1)] !=
                                block_ids_new[std::distance(block_names.begin(), it_tmp)])
      mooseError("In ConcentricCircleMeshGenerator ",
                 _name,
                 ": blocks with different ids cannot have the same block name.");
  }

  for (const auto & elem : mesh0->element_ptr_range())
    for (unsigned i = 0; i < block_ids_old.size(); ++i)
      if (elem->subdomain_id() == block_ids_old[i])
      {
        elem->subdomain_id() = block_ids_new[i];
        break;
      }
  for (unsigned i = 0; i < block_ids_new.size(); ++i)
    mesh0->subdomain_name(block_ids_new[i]) = block_names[i];

  if (_external_boundary_id > 0)
    MooseMesh::changeBoundaryId(*mesh0, OUTER_SIDESET_ID, _external_boundary_id, false);
  if (!_external_boundary_name.empty())
  {
    mesh0->boundary_info->sideset_name(
        _external_boundary_id > 0 ? _external_boundary_id : (boundary_id_type)OUTER_SIDESET_ID) =
        _external_boundary_name;
    mesh0->boundary_info->nodeset_name(
        _external_boundary_id > 0 ? _external_boundary_id : (boundary_id_type)OUTER_SIDESET_ID) =
        _external_boundary_name;
  }
  if (!_interface_boundary_names.empty())
  {
    unsigned int interface_id_shift =
        _has_rings ? (_ring_intervals.front() > 1 ? 2 : 1) : (_background_intervals > 1 ? 2 : 1);
    for (unsigned int i = 0; i < _interface_boundary_names.size(); i++)
    {
      mesh0->boundary_info->sideset_name(i + interface_id_shift + _interface_boundary_id_shift) =
          _interface_boundary_names[i];
      mesh0->boundary_info->nodeset_name(i + interface_id_shift + _interface_boundary_id_shift) =
          _interface_boundary_names[i];
    }
  }
  return dynamic_pointer_cast<MeshBase>(mesh0);
}
