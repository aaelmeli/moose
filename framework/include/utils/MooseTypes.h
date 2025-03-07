//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Moose.h"
#include "ADReal.h"
#include "ADRankTwoTensorForward.h"
#include "ADRankThreeTensorForward.h"
#include "ADRankFourTensorForward.h"

#include "libmesh/libmesh.h"
#include "libmesh/id_types.h"
#include "libmesh/stored_range.h"
#include "libmesh/petsc_macro.h"
#include "libmesh/boundary_info.h"
#include "libmesh/parameters.h"

// BOOST include
#include "bitmask_operators.h"

#include "libmesh/ignore_warnings.h"
#include "Eigen/Core"
#include "libmesh/restore_warnings.h"
#include "libmesh/tensor_tools.h"

#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <functional>

// DO NOT USE (Deprecated)
#define MooseSharedPointer std::shared_ptr
#define MooseSharedNamespace std

/**
 * Macro for inferring the proper type of a normal loop index compatible
 * with the "auto" keyword.
 * Usage:
 *   for (auto i = beginIndex(v); i < v.size(); ++i)    // default index is zero
 *   for (auto i = beginIndex(v, 1); i < v.size(); ++i) // index is supplied
 */
// The multiple macros that you would need anyway [as per: Crazy Eddie (stack overflow)]
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#define beginIndex_0() ERROR-- > "beginIndex() requires one or two arguments"
#define beginIndex_1(A) decltype(A.size())(0)
#define beginIndex_2(A, B) decltype(A.size())(B)
#define beginIndex_3(A, B, C) ERROR-- > "beginIndex() requires one or two arguments"
#define beginIndex_4(A, B, C, D) ERROR-- > "beginIndex() requires one or two arguments"

// The interim macro that simply strips the excess and ends up with the required macro
#define beginIndex_X(x, A, B, C, D, FUNC, ...) FUNC

// The macro that the programmer uses
#define beginIndex(...)                                                                            \
  beginIndex_X(,                                                                                   \
               ##__VA_ARGS__,                                                                      \
               beginIndex_4(__VA_ARGS__),                                                          \
               beginIndex_3(__VA_ARGS__),                                                          \
               beginIndex_2(__VA_ARGS__),                                                          \
               beginIndex_1(__VA_ARGS__),                                                          \
               beginIndex_0(__VA_ARGS__))

/**
 * MooseIndex Macro for creating an index type of the right type for loops and other places where
 * type matching is important.
 * Usage:
 *
 * Type t;
 *
 * Container type:
 * for (MooseIndex(t) i = 0; i < t.size(); ++i)
 *
 * POD type:
 * for (MooseIndex(t) i = 0; i < t; ++i)
 */
#define MooseIndex(type) decltype(_MooseIndex(type, 0))

// SFINAE templates for type MooseIndex type selection
template <typename T, typename std::enable_if<std::is_integral<T>::value>::type * = nullptr>
typename std::remove_const<T>::type
_MooseIndex(T, int)
{
}

template <typename T>
decltype(std::declval<T>().size())
_MooseIndex(T &&, int)
{
}

template <typename T>
decltype("NOTE: MooseIndex only works with integers and objects with size()!")
_MooseIndex(T, double) = delete;

#ifdef __clang__
#pragma clang diagnostic pop
#endif

/**
 * forward declarations
 */
template <typename>
class MooseArray;
template <typename>
class MaterialProperty;
template <typename>
class ADMaterialProperty;
class InputParameters;

namespace libMesh
{
template <typename>
class VectorValue;
typedef VectorValue<Real> RealVectorValue;
typedef Eigen::Matrix<Real, LIBMESH_DIM, 1> RealDIMValue;
typedef Eigen::Matrix<Real, Eigen::Dynamic, 1> RealEigenVector;
typedef Eigen::Matrix<Real, Eigen::Dynamic, LIBMESH_DIM> RealVectorArrayValue;
typedef Eigen::Matrix<Real, Eigen::Dynamic, LIBMESH_DIM * LIBMESH_DIM> RealTensorArrayValue;
typedef Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> RealEigenMatrix;
template <typename>
class TypeVector;
template <typename>
class TensorValue;
typedef TensorValue<Real> RealTensorValue;
template <typename>
class TypeTensor;
template <unsigned int, typename>
class TypeNTensor;
class Point;
template <typename>
class DenseMatrix;
template <typename>
class DenseVector;

namespace TensorTools
{
template <>
struct IncrementRank<Eigen::Matrix<Real, Eigen::Dynamic, 1>>
{
  typedef Eigen::Matrix<Real, Eigen::Dynamic, LIBMESH_DIM> type;
};

template <>
struct IncrementRank<Eigen::Matrix<Real, Eigen::Dynamic, LIBMESH_DIM>>
{
  typedef Eigen::Matrix<Real, Eigen::Dynamic, LIBMESH_DIM * LIBMESH_DIM> type;
};

template <>
struct DecrementRank<Eigen::Matrix<Real, Eigen::Dynamic, LIBMESH_DIM>>
{
  typedef Eigen::Matrix<Real, Eigen::Dynamic, 1> type;
};
}
}

/**
 * various MOOSE typedefs
 */
typedef Real PostprocessorValue;
typedef std::vector<Real> VectorPostprocessorValue;
typedef Real ScatterVectorPostprocessorValue;
typedef boundary_id_type BoundaryID;
typedef unsigned int InterfaceID;
typedef subdomain_id_type SubdomainID;
typedef unsigned int MooseObjectID;
typedef unsigned int THREAD_ID;
typedef unsigned int TagID;
typedef unsigned int TagTypeID;
typedef unsigned int PerfID;
using RestartableDataMapName = std::string; // see MooseApp.h

typedef StoredRange<std::vector<dof_id_type>::iterator, dof_id_type> NodeIdRange;
typedef StoredRange<std::vector<const Elem *>::iterator, const Elem *> ConstElemPointerRange;

namespace Moose
{

/// This is used for places where we initialize some qp-sized data structures
/// that would end up being sized too small after the quadrature order gets
/// bumped (dynamically in-sim).  So for these cases, we just use this constant
/// to size those data structures overly large to accomodate rather than come
/// up with some overkill complex mechanism for dynamically resizing them.
/// Eventually, we may need or implement that more sophisticated mechanism and
/// will no longer need this.
const size_t constMaxQpsPerElem = 216;

// These are used by MooseVariableData and MooseVariableDataFV
enum SolutionState
{
  Current,
  Old,
  Older,
  PreviousNL
};
// These are used by MooseVariableData and MooseVariableDataFV
enum GeometryType
{
  Volume,
  Face
};

template <typename OutputType>
struct ShapeType
{
  typedef OutputType type;
};
template <>
struct ShapeType<Eigen::Matrix<Real, Eigen::Dynamic, 1>>
{
  typedef Real type;
};

template <typename OutputType>
struct DOFType
{
  typedef OutputType type;
};
template <>
struct DOFType<RealVectorValue>
{
  typedef Real type;
};
} // namespace Moose

template <typename OutputType>
struct OutputTools
{
  typedef typename TensorTools::IncrementRank<OutputType>::type OutputGradient;
  typedef typename TensorTools::IncrementRank<OutputGradient>::type OutputSecond;
  typedef typename TensorTools::DecrementRank<OutputType>::type OutputDivergence;

  typedef MooseArray<OutputType> VariableValue;
  typedef MooseArray<OutputGradient> VariableGradient;
  typedef MooseArray<OutputSecond> VariableSecond;
  typedef MooseArray<OutputType> VariableCurl;
  typedef MooseArray<OutputDivergence> VariableDivergence;

  typedef typename Moose::ShapeType<OutputType>::type OutputShape;
  typedef typename TensorTools::IncrementRank<OutputShape>::type OutputShapeGradient;
  typedef typename TensorTools::IncrementRank<OutputShapeGradient>::type OutputShapeSecond;
  typedef typename TensorTools::DecrementRank<OutputShape>::type OutputShapeDivergence;

  typedef MooseArray<std::vector<OutputShape>> VariablePhiValue;
  typedef MooseArray<std::vector<OutputShapeGradient>> VariablePhiGradient;
  typedef MooseArray<std::vector<OutputShapeSecond>> VariablePhiSecond;
  typedef MooseArray<std::vector<OutputShape>> VariablePhiCurl;
  typedef MooseArray<std::vector<OutputShapeDivergence>> VariablePhiDivergence;

  typedef MooseArray<std::vector<OutputShape>> VariableTestValue;
  typedef MooseArray<std::vector<OutputShapeGradient>> VariableTestGradient;
  typedef MooseArray<std::vector<OutputShapeSecond>> VariableTestSecond;
  typedef MooseArray<std::vector<OutputShape>> VariableTestCurl;
  typedef MooseArray<std::vector<OutputShapeDivergence>> VariableTestDivergence;

  // DoF value type for the template class OutputType
  typedef typename Moose::DOFType<OutputType>::type OutputData;
  typedef MooseArray<OutputData> DoFValue;
  typedef OutputType OutputValue;
};

// types for standard variable
typedef typename OutputTools<Real>::VariableValue VariableValue;
typedef typename OutputTools<Real>::VariableGradient VariableGradient;
typedef typename OutputTools<Real>::VariableSecond VariableSecond;
typedef typename OutputTools<Real>::VariableCurl VariableCurl;
typedef typename OutputTools<Real>::VariablePhiValue VariablePhiValue;
typedef typename OutputTools<Real>::VariablePhiGradient VariablePhiGradient;
typedef typename OutputTools<Real>::VariablePhiSecond VariablePhiSecond;
typedef typename OutputTools<Real>::VariablePhiCurl VariablePhiCurl;
typedef typename OutputTools<Real>::VariableTestValue VariableTestValue;
typedef typename OutputTools<Real>::VariableTestGradient VariableTestGradient;
typedef typename OutputTools<Real>::VariableTestSecond VariableTestSecond;
typedef typename OutputTools<Real>::VariableTestCurl VariableTestCurl;

// types for vector variable
typedef typename OutputTools<RealVectorValue>::VariableValue VectorVariableValue;
typedef typename OutputTools<RealVectorValue>::VariableGradient VectorVariableGradient;
typedef typename OutputTools<RealVectorValue>::VariableSecond VectorVariableSecond;
typedef typename OutputTools<RealVectorValue>::VariableCurl VectorVariableCurl;
typedef typename OutputTools<RealVectorValue>::VariablePhiValue VectorVariablePhiValue;
typedef typename OutputTools<RealVectorValue>::VariablePhiGradient VectorVariablePhiGradient;
typedef typename OutputTools<RealVectorValue>::VariablePhiSecond VectorVariablePhiSecond;
typedef typename OutputTools<RealVectorValue>::VariablePhiCurl VectorVariablePhiCurl;
typedef typename OutputTools<RealVectorValue>::VariableTestValue VectorVariableTestValue;
typedef typename OutputTools<RealVectorValue>::VariableTestGradient VectorVariableTestGradient;
typedef typename OutputTools<RealVectorValue>::VariableTestSecond VectorVariableTestSecond;
typedef typename OutputTools<RealVectorValue>::VariableTestCurl VectorVariableTestCurl;

// types for array variable
typedef typename OutputTools<RealEigenVector>::VariableValue ArrayVariableValue;
typedef typename OutputTools<RealEigenVector>::VariableGradient ArrayVariableGradient;
typedef typename OutputTools<RealEigenVector>::VariableSecond ArrayVariableSecond;
typedef typename OutputTools<RealEigenVector>::VariableCurl ArrayVariableCurl;
typedef typename OutputTools<RealEigenVector>::VariablePhiValue ArrayVariablePhiValue;
typedef typename OutputTools<RealEigenVector>::VariablePhiGradient ArrayVariablePhiGradient;
typedef std::vector<std::vector<Eigen::Map<RealDIMValue>>> MappedArrayVariablePhiGradient;
typedef typename OutputTools<RealEigenVector>::VariablePhiSecond ArrayVariablePhiSecond;
typedef typename OutputTools<RealEigenVector>::VariablePhiCurl ArrayVariablePhiCurl;
typedef typename OutputTools<RealEigenVector>::VariableTestValue ArrayVariableTestValue;
typedef typename OutputTools<RealEigenVector>::VariableTestGradient ArrayVariableTestGradient;
typedef typename OutputTools<RealEigenVector>::VariableTestSecond ArrayVariableTestSecond;
typedef typename OutputTools<RealEigenVector>::VariableTestCurl ArrayVariableTestCurl;

/**
 * AD typedefs
 */
typedef libMesh::VectorValue<ADReal> ADRealVectorValue;
typedef ADRealVectorValue ADRealGradient;
typedef libMesh::VectorValue<ADReal> ADPoint;
typedef libMesh::TensorValue<ADReal> ADRealTensorValue;
typedef libMesh::DenseMatrix<ADReal> ADDenseMatrix;
typedef libMesh::DenseVector<ADReal> ADDenseVector;
typedef MooseArray<ADReal> ADVariableValue;
typedef MooseArray<ADRealVectorValue> ADVariableGradient;
typedef MooseArray<ADRealTensorValue> ADVariableSecond;
typedef MooseArray<ADRealVectorValue> ADVectorVariableValue;
typedef MooseArray<ADRealTensorValue> ADVectorVariableGradient;
typedef MooseArray<libMesh::TypeNTensor<3, DualReal>> ADVectorVariableSecond;

namespace Moose
{

// type conversion from regular to AD
template <typename T>
struct ADType;
template <>
struct ADType<Real>
{
  typedef ADReal type;
};
template <>
struct ADType<RankTwoTensor>
{
  typedef ADRankTwoTensor type;
};
template <>
struct ADType<RankThreeTensor>
{
  typedef ADRankThreeTensor type;
};
template <>
struct ADType<RankFourTensor>
{
  typedef ADRankFourTensor type;
};
template <template <typename> class W>
struct ADType<W<Real>>
{
  typedef W<ADReal> type;
};
template <>
struct ADType<RealEigenVector>
{
  typedef RealEigenVector type;
};
template <>
struct ADType<VariableValue>
{
  typedef ADVariableValue type;
};
template <>
struct ADType<VariableGradient>
{
  typedef ADVariableGradient type;
};
template <>
struct ADType<VariableSecond>
{
  typedef ADVariableSecond type;
};

} // namespace Moose

/**
 * some AD typedefs for backwards compatability
 */
typedef ADRealVectorValue DualRealVectorValue;
typedef ADRealTensorValue DualRealTensorValue;
typedef ADRealGradient DualRealGradient;

template <typename T>
using ADTemplateVariableValue =
    typename OutputTools<typename Moose::ADType<T>::type>::VariableValue;
template <typename T>
using ADTemplateVariableGradient =
    typename OutputTools<typename Moose::ADType<T>::type>::VariableGradient;
template <typename T>
using ADTemplateVariableSecond =
    typename OutputTools<typename Moose::ADType<T>::type>::VariableSecond;

typedef VariableTestValue ADVariableTestValue;
typedef VariableTestGradient ADVariableTestGradient;
typedef VariableTestSecond ADVariableTestSecond;
typedef VectorVariableTestValue ADVectorVariableTestValue;
typedef VectorVariableTestGradient ADVectorVariableTestGradient;
typedef VectorVariableTestSecond ADVectorVariableTestSecond;

// We can  use the non-ad version for test values because these don't depend on the mesh
// displacements  (unless the location of the quadrature points depend on the mesh displacements...)
template <typename T>
using ADTemplateVariableTestValue = typename OutputTools<T>::VariableTestValue;
template <typename T>
using ADTemplateVariablePhiValue = typename OutputTools<T>::VariablePhiValue;

// We need to use the AD version for test gradients and seconds because these *do* depend on the
// mesh displacements
template <typename T>
using ADTemplateVariableTestGradient =
    typename OutputTools<typename Moose::ADType<T>::type>::VariableTestGradient;
template <typename T>
using ADTemplateVariableTestSecond =
    typename OutputTools<typename Moose::ADType<T>::type>::VariableTestSecond;
template <typename T>
using ADTemplateVariablePhiGradient =
    typename OutputTools<typename Moose::ADType<T>::type>::VariablePhiGradient;
using ADVariablePhiGradient = ADTemplateVariablePhiGradient<Real>;

// Templated typed to support is_ad templated classes
namespace Moose
{
template <typename T, bool is_ad>
using GenericType = typename std::conditional<is_ad, typename ADType<T>::type, T>::type;
} // namespace Moose

template <bool is_ad>
using GenericReal = typename Moose::GenericType<Real, is_ad>;
template <bool is_ad>
using GenericRealVectorValue = typename Moose::GenericType<RealVectorValue, is_ad>;
template <bool is_ad>
using GenericRankTwoTensor = typename Moose::GenericType<RankTwoTensor, is_ad>;
template <bool is_ad>
using GenericRankThreeTensor = typename Moose::GenericType<RankThreeTensor, is_ad>;
template <bool is_ad>
using GenericRankFourTensor = typename Moose::GenericType<RankFourTensor, is_ad>;
template <bool is_ad>
using GenericVariableValue = typename Moose::GenericType<VariableValue, is_ad>;
template <bool is_ad>
using GenericVariableGradient = typename Moose::GenericType<VariableGradient, is_ad>;
template <bool is_ad>
using GenericVariableSecond = typename Moose::GenericType<VariableSecond, is_ad>;

#define declareADValidParams(ADObjectType)                                                         \
  template <>                                                                                      \
  InputParameters validParams<ADObjectType>()

#define defineADValidParams(ADObjectType, ADBaseObjectType, addedParamCode)                        \
  template <>                                                                                      \
  InputParameters validParams<ADObjectType>()                                                      \
  {                                                                                                \
    InputParameters params = validParams<ADBaseObjectType>();                                      \
    addedParamCode;                                                                                \
    return params;                                                                                 \
  }                                                                                                \
  void mooseClangFormatFunction()

#define defineLegacyParams(ObjectType)                                                             \
  template <>                                                                                      \
  InputParameters validParams<ObjectType>()                                                        \
  {                                                                                                \
    return ObjectType::validParams();                                                              \
  }                                                                                                \
  void mooseClangFormatFunction()

#define defineADLegacyParams(ADObjectType)                                                         \
  template <>                                                                                      \
  InputParameters validParams<ADObjectType>()                                                      \
  {                                                                                                \
    return ADObjectType::validParams();                                                            \
  }                                                                                                \
  void mooseClangFormatFunction()

#define defineADBaseValidParams(ADObjectType, BaseObjectType, addedParamCode)                      \
  template <>                                                                                      \
  InputParameters validParams<ADObjectType>()                                                      \
  {                                                                                                \
    InputParameters params = validParams<BaseObjectType>();                                        \
    addedParamCode;                                                                                \
    return params;                                                                                 \
  }                                                                                                \
  void mooseClangFormatFunction()

#define defineADValidParamsFromEmpty(ADObjectType, addedParamCode)                                 \
  template <>                                                                                      \
  InputParameters validParams<ADObjectType>()                                                      \
  {                                                                                                \
    InputParameters params = emptyInputParameters();                                               \
    addedParamCode;                                                                                \
    return params;                                                                                 \
  }                                                                                                \
  void mooseClangFormatFunction()

namespace Moose
{
extern const processor_id_type INVALID_PROCESSOR_ID;
extern const SubdomainID ANY_BLOCK_ID;
extern const SubdomainID INTERNAL_SIDE_LOWERD_ID;
extern const SubdomainID BOUNDARY_SIDE_LOWERD_ID;
extern const SubdomainID INVALID_BLOCK_ID;
extern const BoundaryID ANY_BOUNDARY_ID;
extern const BoundaryID INVALID_BOUNDARY_ID;
extern const TagID INVALID_TAG_ID;
extern const TagTypeID INVALID_TAG_TYPE_ID;
const std::set<SubdomainID> EMPTY_BLOCK_IDS = {};
const std::set<BoundaryID> EMPTY_BOUNDARY_IDS = {};

/**
 * MaterialData types
 *
 * @see FEProblemBase, MaterialPropertyInterface
 */
enum MaterialDataType
{
  BLOCK_MATERIAL_DATA,
  BOUNDARY_MATERIAL_DATA,
  FACE_MATERIAL_DATA,
  NEIGHBOR_MATERIAL_DATA,
  INTERFACE_MATERIAL_DATA
};

/**
 * Flag for AuxKernel related execution type.
 */
enum AuxGroup
{
  PRE_IC = 0,
  PRE_AUX = 1,
  POST_AUX = 2,
  ALL = 3
};

/**
 * Framework-wide stuff
 */
enum VarKindType
{
  VAR_NONLINEAR,
  VAR_AUXILIARY,
  VAR_ANY
};

enum VarFieldType
{
  VAR_FIELD_STANDARD,
  VAR_FIELD_SCALAR,
  VAR_FIELD_VECTOR,
  VAR_FIELD_ARRAY,
  VAR_FIELD_ANY
};

enum CouplingType
{
  COUPLING_DIAG,
  COUPLING_FULL,
  COUPLING_CUSTOM
};

enum ConstraintSideType
{
  SIDE_PRIMARY,
  SIDE_SECONDARY
};

enum DGResidualType
{
  Element,
  Neighbor
};

enum DGJacobianType
{
  ElementElement,
  ElementNeighbor,
  NeighborElement,
  NeighborNeighbor
};

enum ConstraintType
{
  Secondary = Element,
  Primary = Neighbor
};

enum class ElementType : unsigned int
{
  Element = DGResidualType::Element,
  Neighbor = DGResidualType::Neighbor,
  Lower = DGResidualType::Neighbor + 1
};

enum class MortarType : unsigned int
{
  Secondary = static_cast<unsigned int>(Moose::ElementType::Element),
  Primary = static_cast<unsigned int>(Moose::ElementType::Neighbor),
  Lower = static_cast<unsigned int>(Moose::ElementType::Lower)
};

/**
 * The filter type applied to a particular piece of "restartable" data. These filters
 * will be applied during deserialization to include or exclude data as appropriate.
 */
enum class RESTARTABLE_FILTER : unsigned char
{
  RECOVERABLE
};

enum ConstraintJacobianType
{
  SecondarySecondary = ElementElement,
  SecondaryPrimary = ElementNeighbor,
  PrimarySecondary = NeighborElement,
  PrimaryPrimary = NeighborNeighbor,
  LowerLower,
  LowerSecondary,
  LowerPrimary,
  SecondaryLower,
  PrimaryLower
};

enum CoordinateSystemType
{
  COORD_XYZ,
  COORD_RZ,
  COORD_RSPHERICAL
};

/**
 * Preconditioning side
 */
enum PCSideType
{
  PCS_LEFT,
  PCS_RIGHT,
  PCS_SYMMETRIC,
  PCS_DEFAULT ///< Use whatever we have in PETSc
};

/**
 * Norm type for converge test
 */
enum MooseKSPNormType
{
  KSPN_NONE,
  KSPN_PRECONDITIONED,
  KSPN_UNPRECONDITIONED,
  KSPN_NATURAL,
  KSPN_DEFAULT ///< Use whatever we have in PETSc
};

/**
 * Type of the solve
 */
enum SolveType
{
  ST_PJFNK,  ///< Preconditioned Jacobian-Free Newton Krylov
  ST_JFNK,   ///< Jacobian-Free Newton Krylov
  ST_NEWTON, ///< Full Newton Solve
  ST_FD,     ///< Use finite differences to compute Jacobian
  ST_LINEAR  ///< Solving a linear problem
};

/**
 * Type of the eigen solve
 */
enum EigenSolveType
{
  EST_POWER,           ///< Power / Inverse / RQI
  EST_ARNOLDI,         ///< Arnoldi
  EST_KRYLOVSCHUR,     ///< Krylov-Schur
  EST_JACOBI_DAVIDSON, ///< Jacobi-Davidson
  EST_NONLINEAR_POWER, ///< Nonlinear inverse power
  EST_NEWTON, ///< Newton-based eigensolver with an assembled Jacobian matrix (fully coupled by default)
  EST_PJFNK, ///< Preconditioned Jacobian-free Newton Krylov
  EST_JFNK   ///< Jacobian-free Newton Krylov
};

/**
 * Type of the eigen problem
 */
enum EigenProblemType
{
  EPT_HERMITIAN,             ///< Hermitian
  EPT_NON_HERMITIAN,         ///< Non-Hermitian
  EPT_GEN_HERMITIAN,         ///< Generalized Hermitian
  EPT_GEN_INDEFINITE,        ///< Generalized Hermitian indefinite
  EPT_GEN_NON_HERMITIAN,     ///< Generalized Non-Hermitian
  EPT_POS_GEN_NON_HERMITIAN, ///< Generalized Non-Hermitian with positive (semi-)definite B
  EPT_SLEPC_DEFAULT          ///< use whatever SLPEC has by default
};

/**
 * Which eigen pairs
 */
enum WhichEigenPairs
{
  WEP_LARGEST_MAGNITUDE,  ///< largest magnitude
  WEP_SMALLEST_MAGNITUDE, ///< smallest magnitude
  WEP_LARGEST_REAL,       ///< largest real
  WEP_SMALLEST_REAL,      ///< smallest real
  WEP_LARGEST_IMAGINARY,  ///< largest imaginary
  WEP_SMALLEST_IMAGINARY, ///< smallest imaginary
  WEP_TARGET_MAGNITUDE,   ///< target magnitude
  WEP_TARGET_REAL,        ///< target real
  WEP_TARGET_IMAGINARY,   ///< target imaginary
  WEP_ALL_EIGENVALUES,    ///< all eigenvalues
  WEP_SLEPC_DEFAULT       ///< use whatever we have in SLEPC
};

/**
 * Time integrators
 */
enum TimeIntegratorType
{
  TI_IMPLICIT_EULER,
  TI_EXPLICIT_EULER,
  TI_CRANK_NICOLSON,
  TI_BDF2,
  TI_EXPLICIT_MIDPOINT,
  TI_LSTABLE_DIRK2,
  TI_EXPLICIT_TVD_RK_2,
  TI_NEWMARK_BETA
};

/**
 * Type of constraint formulation
 */
enum ConstraintFormulationType
{
  Penalty,
  Kinematic
};
/**
 * Type of the line search
 */
enum LineSearchType
{
  LS_INVALID, ///< means not set
  LS_DEFAULT,
  LS_NONE,
  LS_BASIC,
#if PETSC_VERSION_LESS_THAN(3, 3, 0)
  LS_CUBIC,
  LS_QUADRATIC,
  LS_BASICNONORMS,
#else
  LS_SHELL,
  LS_CONTACT,
  LS_PROJECT,
  LS_L2,
  LS_BT,
  LS_CP
#endif
};

/**
 * Type of the matrix-free finite-differencing parameter
 */
enum MffdType
{
  MFFD_INVALID, ///< means not set
  MFFD_WP,
  MFFD_DS
};

/**
 * Type of patch update strategy for modeling node-face constraints or contact
 */
enum PatchUpdateType
{
  Never,
  Always,
  Auto,
  Iteration
};

/**
 * Main types of Relationship Managers
 */
enum class RelationshipManagerType : unsigned char
{
  DEFAULT = 0,
  GEOMETRIC = 1 << 0,
  ALGEBRAIC = 1 << 1,
  COUPLING = 1 << 2
};

enum RMSystemType
{
  NONLINEAR,
  AUXILIARY,
  NONE
};

enum VectorTagType
{
  VECTOR_TAG_RESIDUAL = 0,
  VECTOR_TAG_SOLUTION = 1,
  VECTOR_TAG_ANY = 2
};

/**
 * The type for the callback to set RelationshipManager parameters
 */
typedef std::function<void(const InputParameters &, InputParameters &)>
    RelationshipManagerInputParameterCallback;

std::string stringify(const Moose::RelationshipManagerType & t);
} // namespace Moose

namespace libMesh
{
template <>
inline void
print_helper(std::ostream & os, const Moose::RelationshipManagerType * param)
{
  // Specialization so that we don't print out unprintable characters
  os << Moose::stringify(*param);
}

// End of Moose Namespace
}

template <>
struct enable_bitmask_operators<Moose::RelationshipManagerType>
{
  static const bool enable = true;
};

/**
 * This Macro is used to generate std::string derived types useful for
 * strong type checking and special handling in the GUI.  It does not
 * extend std::string in any way so it is generally "safe"
 */
#define DerivativeStringClass(TheName)                                                             \
  class TheName : public std::string                                                               \
  {                                                                                                \
  public:                                                                                          \
    TheName() : std::string() {}                                                                   \
    TheName(const std::string & str) : std::string(str) {}                                         \
    TheName(const std::string & str, size_t pos, size_t n = npos) : std::string(str, pos, n) {}    \
    TheName(const char * s, size_t n) : std::string(s, n) {}                                       \
    TheName(const char * s) : std::string(s) {}                                                    \
    TheName(size_t n, char c) : std::string(n, c) {}                                               \
  } /* No semicolon here because this is a macro */

// Instantiate new Types

/// This type is for expected (i.e. input) file names or paths that your simulation needs.  If
/// relative paths are assigned to this type, they are treated/modified to be relative to the
/// location of the simulation's main input file's directory.  It can be used to trigger open file
/// dialogs in the GUI.
DerivativeStringClass(FileName);

/// This type is for expected filenames where the extension is unwanted, it can be used to trigger open file dialogs in the GUI
DerivativeStringClass(FileNameNoExtension);

/// This type is similar to "FileName", but is used to further filter file dialogs on known file mesh types
DerivativeStringClass(MeshFileName);

/// This type is for output file base
DerivativeStringClass(OutFileBase);

/// This type is used for objects that expect nonlinear variable names (i.e. Kernels, BCs)
DerivativeStringClass(NonlinearVariableName);

/// This type is used for objects that expect Auxiliary variable names (i.e. AuxKernels, AuxBCs)
DerivativeStringClass(AuxVariableName);

/// This type is used for objects that expect either Nonlinear or Auxiliary Variables such as postprocessors
DerivativeStringClass(VariableName);

/// This type is used for objects that expect Boundary Names/Ids read from or generated on the current mesh
DerivativeStringClass(BoundaryName);

/// This type is similar to BoundaryName but is used for "blocks" or subdomains in the current mesh
DerivativeStringClass(SubdomainName);

/// This type is used for objects that expect Postprocessor objects
DerivativeStringClass(PostprocessorName);

/// This type is used for objects that expect VectorPostprocessor objects
DerivativeStringClass(VectorPostprocessorName);

/// This type is used for objects that expect Moose Function objects
DerivativeStringClass(FunctionName);

/// This type is used for objects that expect Moose Distribution objects
DerivativeStringClass(DistributionName);

/// This type is used for objects that expect Moose Sampler objects
DerivativeStringClass(SamplerName);

/// This type is used for objects that expect "UserObject" names
DerivativeStringClass(UserObjectName);

/// This type is used for objects that expect an Indicator object name
DerivativeStringClass(IndicatorName);

/// This type is used for objects that expect an Marker object name
DerivativeStringClass(MarkerName);

/// This type is used for objects that expect an MultiApp object name
DerivativeStringClass(MultiAppName);

/// Used for objects the require Output object names
DerivativeStringClass(OutputName);

/// Used for objects that expect MaterialProperty names
DerivativeStringClass(MaterialPropertyName);

/// Used for objects that expect Moose::Functor names
DerivativeStringClass(MooseFunctorName);

/// User for accessing Material objects
DerivativeStringClass(MaterialName);

/// Tag Name
DerivativeStringClass(TagName);

/// Name of MeshGenerators
DerivativeStringClass(MeshGeneratorName);

/// Name of extra element IDs
DerivativeStringClass(ExtraElementIDName);

/// Name of a Reporter Value, second argument to ReporterName (see Reporter.h)
DerivativeStringClass(ReporterValueName);

namespace Moose
{
extern const TagName SOLUTION_TAG;
extern const TagName OLD_SOLUTION_TAG;
extern const TagName OLDER_SOLUTION_TAG;
extern const TagName PREVIOUS_NL_SOLUTION_TAG;
}
