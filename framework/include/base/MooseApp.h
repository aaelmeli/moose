//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

// MOOSE includes
#include "Moose.h"
#include "Parser.h"
#include "ActionWarehouse.h"
#include "Factory.h"
#include "ActionFactory.h"
#include "OutputWarehouse.h"
#include "RestartableData.h"
#include "ConsoleStreamInterface.h"
#include "PerfGraph.h"
#include "PerfGraphInterface.h"
#include "TheWarehouse.h"
#include "RankMap.h"

#include "libmesh/parallel_object.h"
#include "libmesh/mesh_base.h"
#include "libmesh/point.h"

// C++ includes
#include <list>
#include <map>
#include <set>
#include <unordered_set>

// Forward declarations
class Executioner;
class MooseApp;
class Backup;
class FEProblemBase;
class MeshGenerator;
class InputParameterWarehouse;
class SystemInfo;
class CommandLine;
class RelationshipManager;
namespace libMesh
{
class ExodusII_IO;
}

template <>
InputParameters validParams<MooseApp>();

/**
 * Base class for MOOSE-based applications
 *
 * This generic class for application provides:
 * - parsing command line arguments,
 * - parsing an input file,
 * - executing the simulation
 *
 * Each application should register its own objects and register its own special syntax
 */
class MooseApp : public ConsoleStreamInterface,
                 public PerfGraphInterface,
                 public libMesh::ParallelObject
{
public:
  static const RestartableDataMapName MESH_META_DATA;

  static InputParameters validParams();

  virtual ~MooseApp();

  TheWarehouse & theWarehouse() { return *_the_warehouse; }

  /**
   * Get the name of the object. In the case of MooseApp, the name of the object is *NOT* the name
   * of the application. It's the name of the created application which is usually "main". If you
   * have subapps, then each individual subapp will have a unique name which typically comes from
   * the input file (e.g. sub0, sub1, etc...).
   * @return The name of the object
   */
  const std::string & name() const { return _name; }

  virtual void checkRegistryLabels();

  /**
   * Get printable name of the application.
   */
  virtual std::string getPrintableName() const { return "Application"; }

  virtual std::string appBinaryName() const
  {
    auto name = Moose::getExecutableName();
    name = name.substr(0, name.find_last_of("-"));
    if (name.find_first_of("/") != std::string::npos)
      name = name.substr(name.find_first_of("/") + 1, std::string::npos);
    return name;
  }

  /**
   * Get the parameters of the object
   * @return The parameters of the object
   */
  InputParameters & parameters() { return _pars; }

  /**
   * Get the type of this object as a string. This is a string version of the class name (e.g.
   * MooseTestApp).
   * @return The the type of the object
   */
  const std::string & type() const { return _type; }

  /**
   * The RankMap is a useful object for determining how the processes
   * are laid out on the physical nodes of the cluster
   */
  const RankMap & rankMap() { return _rank_map; }

  /**
   * Get the PerfGraph for this app
   */
  PerfGraph & perfGraph() { return _perf_graph; }

  ///@{
  /**
   * Retrieve a parameter for the object
   * @param name The name of the parameter
   * @return The value of the parameter
   */
  template <typename T>
  const T & getParam(const std::string & name);

  template <typename T>
  const T & getParam(const std::string & name) const;
  ///@}

  inline bool isParamValid(const std::string & name) const { return _pars.isParamValid(name); }

  /**
   * Run the application
   */
  virtual void run();

  /**
   * Returns the framework version.
   */
  std::string getFrameworkVersion() const;

  /**
   * Returns the current version of the framework or application (default: framework version).
   */
  virtual std::string getVersion() const;

  /**
   * Non-virtual method for printing out the version string in a consistent format.
   */
  std::string getPrintableVersion() const;

  /**
   * Setup options based on InputParameters.
   */
  virtual void setupOptions();

  /**
   * Return a writable reference to the ActionWarehouse associated with this app
   */
  ActionWarehouse & actionWarehouse() { return _action_warehouse; }
  /**
   * Return a const reference to the ActionWarehouse associated with this app
   */
  const ActionWarehouse & actionWarehouse() const { return _action_warehouse; }

  /**
   * Returns a writable reference to the parser
   */
  Parser & parser() { return _parser; }

  /**
   * Returns a writable reference to the syntax object.
   */
  Syntax & syntax() { return _syntax; }

  /**
   * Set the input file name.
   */
  void setInputFileName(const std::string & input_file_name);

  /**
   * Returns the input file name that was set with setInputFileName
   */
  std::string getInputFileName() const
  {
    return _input_filenames.empty() ? "" : _input_filenames.back();
  }

  /**
   * Override the selection of the output file base name.
   * Note: This method is supposed to be called by MultiApp only.
   */
  void setOutputFileBase(const std::string & output_file_base)
  {
    _output_file_base = output_file_base;
    _file_base_set_by_user = true;
  }

  /**
   * Get the output file base name.
   * @param for_non_moose_build_output True for getting the file base for outputs generated with
   *                                   Outputs/[outputname] input syntax.
   * @return The file base name used by output objects
   * Note: for_non_moose_build_output does not affect the returned value when this is a subapp.
   *       for_non_moose_build_output also does not affect the returned value when Outputs/file_base
   *       parameter is available. When for_non_moose_build_output does affect the returned value,
   *       i.e. master without Outputs/file_base, the suffix _out is removed.
   */
  std::string getOutputFileBase(bool for_non_moose_build_output = false) const;

  /**
   * Tell the app to output in a specific position.
   */
  void setOutputPosition(const Point & p);

  /**
   * Get all checkpoint directories
   * @return A Set of checkpoint directories
   */
  std::list<std::string> getCheckpointDirectories() const;

  /**
   * Extract all possible checkpoint file names
   * @return A Set of checkpoint filenames
   */
  std::list<std::string> getCheckpointFiles() const;

  /**
   * Whether or not an output position has been set.
   * @return True if it has
   */
  bool hasOutputPosition() const { return _output_position_set; }

  /**
   * Get the output position.
   * @return The position offset for the output.
   */
  Point getOutputPosition() const { return _output_position; }

  /**
   * Set the starting time for the simulation.  This will override any choice
   * made in the input file.
   *
   * @param time The start time for the simulation.
   */
  void setStartTime(Real time);

  /**
   * @return Whether or not a start time has been programmatically set using setStartTime()
   */
  bool hasStartTime() const { return _start_time_set; }

  /**
   * @return The start time
   */
  Real getStartTime() const { return _start_time; }

  /**
   * Each App has it's own local time.  The "global" time of the whole problem might be
   * different.  This offset is how far off the local App time is from the global time.
   */
  void setGlobalTimeOffset(Real offset) { _global_time_offset = offset; }

  /**
   * Each App has it's own local time.  The "global" time of the whole problem might be
   * different.  This offset is how far off the local App time is from the global time.
   */
  Real getGlobalTimeOffset() const { return _global_time_offset; }

  /**
   * Return the primary (first) filename that was parsed
   * Note: When stripLeadingPath is false, this function returns the same name as
   *       getInputFileName() method when the input file is not a link.
   */
  std::string getFileName(bool stripLeadingPath = true) const;

  /**
   * Set a flag so that the parser will either warn or error when unused variables are seen after
   * parsing is complete.
   */
  void setCheckUnusedFlag(bool warn_is_error = false);

  /**
   * Set a flag so that the parser will throw an error if overridden parameters are detected
   */
  void setErrorOverridden();

  /**
   * Removes warnings and error checks for unrecognized variables in the input file
   */
  void disableCheckUnusedFlag();

  /**
   * Retrieve the Executioner for this App
   */
  Executioner * getExecutioner() const { return _executioner.get(); }

  /**
   * Set the Executioner for this App
   */
  void setExecutioner(std::shared_ptr<Executioner> && executioner) { _executioner = executioner; }

  /**
   * Returns a writable Boolean indicating whether this app will use a Nonlinear or Eigen System.
   */
  bool & useNonlinear() { return _use_nonlinear; }

  /**
   * Returns a writable Boolean indicating whether this app will use an eigenvalue executioner.
   */
  bool & useEigenvalue() { return _use_eigen_value; }

  /**
   * Retrieve a writable reference to the Factory associated with this App.
   */
  Factory & getFactory() { return _factory; }

  /**
   * Retrieve a writable reference to the ActionFactory associated with this App.
   */
  ActionFactory & getActionFactory() { return _action_factory; }

  /**
   * Returns the MPI processor ID of the current processor.
   */
  processor_id_type processor_id() const { return _comm->rank(); }

  /**
   * Get the command line
   * @return The reference to the command line object
   * Setup options based on InputParameters.
   */
  std::shared_ptr<CommandLine> commandLine() const { return _command_line; }

  /**
   * Set the flag to indicate whether or not we need to use a separate Exodus reader to read the
   * mesh BEFORE we create the mesh.
   */
  void setExodusFileRestart(bool flag) { _initial_from_file = flag; }

  /**
   * Whether or not we need to use a separate Exodus reader to read the mesh BEFORE we create the
   * mesh.
   */
  bool getExodusFileRestart() const { return _initial_from_file; }

  /**
   * Set the Exodus reader to restart variables from an Exodus mesh file
   */
  void setExReaderForRestart(std::shared_ptr<ExodusII_IO> && exreader) { _ex_reader = exreader; }

  /**
   * Get the Exodus reader to restart variables from an Exodus mesh file
   */
  ExodusII_IO * getExReaderForRestart() const { return _ex_reader.get(); }

  /**
   * Actually build everything in the input file.
   */
  virtual void runInputFile();

  /**
   * Execute the Executioner that was built.
   */
  virtual void executeExecutioner();

  /**
   * Returns true if the user specified --distributed-mesh (or
   * --parallel-mesh, for backwards compatibility) on the command line
   * and false otherwise.
   */
  bool getDistributedMeshOnCommandLine() const { return _distributed_mesh_on_command_line; }

  /**
   * Whether or not this is a "recover" calculation.
   */
  bool isRecovering() const;

  /**
   * Whether or not this is a "restart" calculation.
   */
  bool isRestarting() const;

  /**
   * Whether or not this is a split mesh operation.
   */
  bool isSplitMesh() const;

  /**
   * Whether or not we are running with pre-split (distributed mesh)
   */
  bool isUseSplit() const;

  ///@{
  /**
   * Return true if the recovery file base is set
   */
  bool hasRestartRecoverFileBase() const;
  bool hasRecoverFileBase() const;
  ///@}

  ///@{
  /**
   * The file_base for the recovery file.
   */
  std::string getRestartRecoverFileBase() const { return _restart_recover_base; }
  std::string getRecoverFileBase() const
  {
    mooseDeprecated("MooseApp::getRecoverFileBase is deprecated, use "
                    "MooseApp::getRestartRecoverFileBase() instead.");
    return _restart_recover_base;
  }
  ///@}

  /**
   * mutator for recover_base (set by RecoverBaseAction)
   */
  void setRestartRecoverFileBase(const std::string & file_base)
  {
    if (file_base.empty())
      _restart_recover_base = MooseUtils::getLatestAppCheckpointFileBase(getCheckpointFiles());
    else
      _restart_recover_base = file_base;
  }

  /**
   * The suffix for the recovery file.
   */
  std::string getRestartRecoverFileSuffix() const { return _restart_recover_suffix; }

  /**
   * mutator for recover_suffix
   */
  void setRestartRecoverFileSuffix(const std::string & file_suffix)
  {
    _restart_recover_suffix = file_suffix;
  }

  /**
   *  Whether or not this simulation should only run half its transient (useful for testing
   * recovery)
   */
  bool halfTransient() const { return _half_transient; }

  /**
   * Store a map of outputter names and file numbers
   * The MultiApp system requires this to get the file numbering to propagate down through the
   * Multiapps.
   * @param numbers Map of outputter names and file numbers
   *
   * @see MultiApp TransientMultiApp OutputWarehouse
   */
  void setOutputFileNumbers(const std::map<std::string, unsigned int> & numbers)
  {
    _output_file_numbers = numbers;
  }

  /**
   * Store a map of outputter names and file numbers
   * The MultiApp system requires this to get the file numbering to propogate down through the
   * multiapps.
   *
   * @see MultiApp TransientMultiApp
   */
  const std::map<std::string, unsigned int> & getOutputFileNumbers() const
  {
    return _output_file_numbers;
  }

  /**
   * Get the OutputWarehouse objects
   */
  OutputWarehouse & getOutputWarehouse();

  /**
   * Get SystemInfo object
   * @return A pointer to the SystemInformation object
   */
  const SystemInfo * getSystemInfo() const { return _sys_info.get(); }

  ///@{
  /**
   * Thes methods are called to register applications or objects on demand. This method
   * attempts to load a dynamic library and register it when it is needed. Throws an error if
   * no suitable library is found that contains the app_name in question.
   */
  void dynamicAllRegistration(const std::string & app_name,
                              Factory * factory,
                              ActionFactory * action_factory,
                              Syntax * syntax,
                              std::string library_path,
                              const std::string & library_name);
  void dynamicAppRegistration(const std::string & app_name,
                              std::string library_path,
                              const std::string & library_name);
  ///@}

  /**
   * Converts an application name to a library name:
   * Examples:
   *   AnimalApp -> libanimal-oprof.la (assuming METHOD=oprof)
   *   ThreeWordAnimalApp -> libthree_word_animal-dbg.la (assuming METHOD=dbg)
   */
  std::string appNameToLibName(const std::string & app_name) const;

  /**
   * Converts a library name to an application name:
   */
  std::string libNameToAppName(const std::string & library_name) const;

  /**
   * Return the loaded library filenames in a std::set
   */
  std::set<std::string> getLoadedLibraryPaths() const;

  /**
   * Get the InputParameterWarehouse for MooseObjects
   */
  InputParameterWarehouse & getInputParameterWarehouse();

  /*
   * Register a piece of restartable data.  This is data that will get
   * written / read to / from a restart file.
   *
   * @param name The full (unique) name.
   * @param data The actual data object.
   * @param tid The thread id of the object.  Use 0 if the object is not threaded.
   * @param read_only Restrict the data for read-only
   * @param metaname (optional) register the data to the meta data storage (tid must be 0)
   */
  RestartableDataValue & registerRestartableData(const std::string & name,
                                                 std::unique_ptr<RestartableDataValue> data,
                                                 THREAD_ID tid,
                                                 bool read_only,
                                                 const RestartableDataMapName & metaname = "");

  /*
   * Check if a restartable meta data exists or not.
   *
   * @param name The full (unique) name.
   * @param metaname The name to the meta data storage
   */
  bool hasRestartableMetaData(const std::string & name,
                              const RestartableDataMapName & metaname) const;

  /**
   * Return reference to the restartable data object
   * @return A const reference to the restartable data object
   */
  const RestartableDataMaps & getRestartableData() const { return _restartable_data; }

  /**
   * Return a reference to restartable data for the specific type flag.
   */
  const RestartableDataMap & getRestartableDataMap(const RestartableDataMapName & name) const;

  /**
   * Reserve a location for storing custom RestartableDataMap objects.
   *
   * This should be called in the constructor of an application.
   *
   * @param name A key to use for accessing the data object
   * @param suffix The suffix to use when appending to checkpoint output, if not supplied the
   *               given name is used to generate the suffix (MyMetaData -> _mymetadata)
   */
  void registerRestartableDataMapName(const RestartableDataMapName & name, std::string suffix = "");

  /**
   * Return a reference to the recoverable data object
   * @return A const reference to the recoverable data
   */
  const DataNames & getRecoverableData() const { return _recoverable_data_names; }

  /**
   * Create a Backup from the current App. A Backup contains all the data necessary to be able to
   * restore the state of an App.
   *
   * This method should be overridden in external or MOOSE-wrapped applications.
   */
  virtual std::shared_ptr<Backup> backup();

  /**
   * Restore a Backup. This sets the App's state.
   *
   * @param backup The Backup holding the data for the app
   * @param for_restart Whether this restoration is explicitly for the first restoration of restart
   * data.
   *
   * This method should be overridden in external or MOOSE-wrapped applications.
   */
  virtual void restore(std::shared_ptr<Backup> backup, bool for_restart = false);

  /**
   * Returns a string to be printed at the beginning of a simulation
   */
  virtual std::string header() const;

  /**
   * The MultiApp Level
   * @return The current number of levels from the master app
   */
  unsigned int multiAppLevel() const { return _multiapp_level; }

  /**
   * The MultiApp number
   * @return The numbering in all the sub-apps on the same level
   */
  unsigned int multiAppNumber() const { return _multiapp_number; }

  /**
   * Whether or not this app is the ultimate master app. (ie level == 0)
   */
  bool isUltimateMaster() { return !_multiapp_level; }

  /**
   * Returns a pointer to the master mesh
   */
  const MooseMesh * masterMesh() const { return _master_mesh; }

  /**
   * Returns a pointer to the master mesh
   */
  const MooseMesh * masterDisplacedMesh() const { return _master_displaced_mesh; }

  /**
   * Set final mesh generator name
   */
  void setFinalMeshGeneratorName(const std::string & generator_name);

  /**
   * Add a mesh generator that will act on the meshes in the system
   */
  void addMeshGenerator(const std::string & generator_name,
                        const std::string & name,
                        InputParameters parameters);

  /**
   * Get a mesh generator with its name
   */
  const MeshGenerator & getMeshGenerator(const std::string & name) const;

  /**
   * Get names of all mesh generators
   * Note: This function should be called after all mesh generators are added with the
   * 'add_mesh_generator' task. The returned value will be undefined and depends on the ordering
   * that mesh generators are added by MOOSE if the function is called during the
   * 'add_mesh_generator' task.
   */
  std::vector<std::string> getMeshGeneratorNames() const;

  /**
   * Get a refernce to a pointer that will be the output of the
   * MeshGenerator named name
   */
  std::unique_ptr<MeshBase> & getMeshGeneratorOutput(const std::string & name);

  /**
   * Append a mesh generator that will act on the final mesh generator in the system
   * Note: This function must be called after add_mesh_generator task.
   */
  void appendMeshGenerator(const std::string & generator_name,
                           const std::string & name,
                           InputParameters parameters);

  /**
   * Clear all mesh modifers
   */
  void clearMeshGenerators();

  /**
   * Execute and clear the Mesh Generators data structure
   */
  void executeMeshGenerators();

  /**
   * Whether this app is executing mesh generators
   */
  bool executingMeshGenerators() const { return _executing_mesh_generators; }

  /**
   * Get the generated mesh generated by executeMeshGenerators();
   */
  std::unique_ptr<MeshBase> getMeshGeneratorMesh(bool check_unique = true);

  ///@{
  /**
   * Sets the restart/recover flags
   * @param state The state to set the flag to
   */
  void setRestart(bool value);
  void setRecover(bool value);
  ///@}

  /// Returns whether the Application is running in check input mode
  bool checkInput() const { return _check_input; }

  /// Returns whether FPE trapping is turned on (either because of debug or user requested)
  bool getFPTrapFlag() const { return _trap_fpe; }

  /**
   * WARNING: This is an internal method for MOOSE, if you need the add new ExecFlagTypes then
   * use the registerExecFlag macro as done in Moose.C/h.
   *
   * @param flag The flag to add as available to the app level ExecFlagEnum.
   */
  void addExecFlag(const ExecFlagType & flag);

  /**
   * Returns a Boolean indicating whether a RelationshipManater exists with the same name.
   */
  bool hasRelationshipManager(const std::string & name) const;

  /**
   * Transfers ownership of a RelationshipManager to the application for lifetime management.
   * The RelationshipManager will NOT be duplicately added if an equivalent RelationshipManager
   * is already active. In that case, it's possible that the object will be destroyed if the
   * reference count drops to zero.
   */
  bool addRelationshipManager(std::shared_ptr<RelationshipManager> relationship_manager);

private:
  /**
   * Purge this relationship manager from meshes and DofMaps and finally from us. This method is
   * private because only this object knows when we should remove relationship managers: when we are
   * adding relationship managers to this object's storage, we perform an operator>= comparison
   * between our existing RMs and the RM we are trying to add. If any comparison returns true, we do
   * not add the new RM because the comparison indicates that we would gain no new coverage.
   * However, if no comparison return true, then we add the new RM and we turn the comparison
   * around! Consequently if our new RM is >= than any of our preexisting RMs, we remove those
   * preexisting RMs using this method
   */
  void removeRelationshipManager(std::shared_ptr<RelationshipManager> relationship_manager);

public:
  /**
   * Attach the relationship managers of the given type
   * Note: Geometric relationship managers that are supposed to be attached late
   * will be attached when Algebraic are attached.
   */
  void attachRelationshipManagers(Moose::RelationshipManagerType rm_type,
                                  bool attach_geometric_rm_final = false);

  /**
   * Attach geometric relationship managers to the given \p MeshBase object. This API is designed to
   * work with \p MeshGenerators which are executed at the very beginning of a simulation. No
   * attempt will be made to add relationship managers to a displaced mesh, because it doesn't exist
   * yet.
   */
  void attachRelationshipManagers(MeshBase & mesh, MooseMesh & moose_mesh);

  /**
   * Retrieve the relationship managers
   */
  const std::vector<std::shared_ptr<RelationshipManager>> & getReleationshipManagers();

  /**
   * Returns the Relationship managers info suitable for printing.
   */
  std::vector<std::pair<std::string, std::string>> getRelationshipManagerInfo() const;

  /**
   * Return the app level ExecFlagEnum, this contains all the available flags for the app.
   */
  const ExecFlagEnum & getExecuteOnEnum() const { return _execute_flags; }

  /**
   * Method for setting the backup object to be restored at a later time. This method is called
   * during simulation restart or recover before the application is completely setup. The backup
   * object set here, will be restored when needed by a call to restoreCachedBackup().
   *
   * @param backup The Backup holding the data for the app.
   */
  void setBackupObject(std::shared_ptr<Backup> backup);

  /**
   * Whether to enable automatic scaling by default
   */
  bool defaultAutomaticScaling() const { return _automatic_automatic_scaling; }

  // Return the communicator for this application
  const std::shared_ptr<Parallel::Communicator> getCommunicator() const { return _comm; }

  /**
   * Return the container of relationship managers
   */
  const std::set<std::shared_ptr<RelationshipManager>> & relationshipManagers() const
  {
    return _relationship_managers;
  }

  /**
   * Function to check the integrity of the restartable meta data structure
   */
  void checkMetaDataIntegrity() const;

  ///@{
  /**
   * Iterator based access to the extra RestartableDataMap objects; see Checkpoint.C for use case.
   *
   * These are MOOSE internal functions and should not be used otherwise.
   */
  std::unordered_map<RestartableDataMapName,
                     std::pair<RestartableDataMap, std::string>>::const_iterator
  getRestartableDataMapBegin() const
  {
    return _restartable_meta_data.begin();
  }

  std::unordered_map<RestartableDataMapName,
                     std::pair<RestartableDataMap, std::string>>::const_iterator
  getRestartableDataMapEnd() const
  {
    return _restartable_meta_data.end();
  }
  ///@}

  /**
   * Whether this application should by default error on Jacobian nonzero reallocations. The
   * application level setting can always be overridden by setting the \p
   * error_on_jacobian_nonzero_reallocation parameter in the \p Problem block of the input file
   */
  virtual bool errorOnJacobianNonzeroReallocation() const { return false; }

protected:
  /**
   * Whether or not this MooseApp has cached a Backup to use for restart / recovery
   */
  bool hasCachedBackup() const { return _cached_backup.get(); }

  /**
   * Restore from a cached backup
   */
  void restoreCachedBackup();

  /**
   * Helper method for dynamic loading of objects
   */
  void dynamicRegistration(const Parameters & params);

  /**
   * Recursively loads libraries and dependencies in the proper order to fully register a
   * MOOSE application that may have several dependencies. REQUIRES: dynamic linking loader support.
   */
  void loadLibraryAndDependencies(const std::string & library_filename, const Parameters & params);

  /// Constructor is protected so that this object is constructed through the AppFactory object
  MooseApp(InputParameters parameters);

  /**
   * NOTE: This is an internal function meant for MOOSE use only!
   *
   * Register a piece of restartable data that will be used in a filter in/out during
   * deserialization. Note however that this data will always be written to the restart file.
   *
   * @param name The full (unique) name.
   * @param filter The filter name where to direct the name
   */
  void registerRestartableNameWithFilter(const std::string & name,
                                         Moose::RESTARTABLE_FILTER filter);

  /**
   * Runs post-initialization error checking that cannot be run correctly unless the simulation
   * has been fully set up and initialized.
   */
  void errorCheck();

  /// The name of this object
  const std::string _name;

  /// Parameters of this object
  InputParameters _pars;

  /// The string representation of the type of this object as registered (see registerApp(AppName))
  const std::string _type;

  /// The MPI communicator this App is going to use
  const std::shared_ptr<Parallel::Communicator> _comm;

  /// Input file names used
  std::vector<std::string> _input_filenames;

  /// The output file basename
  std::string _output_file_base;

  /// Whether or not file base is set through input or setOutputFileBase by MultiApp
  bool _file_base_set_by_user;

  /// Whether or not an output position has been set for this app
  bool _output_position_set;

  /// The output position
  Point _output_position;

  /// Whether or not an start time has been set
  bool _start_time_set;

  /// The time at which to start the simulation
  Real _start_time;

  /// Offset of the local App time to the "global" problem time
  Real _global_time_offset;

  /// Command line object
  std::shared_ptr<CommandLine> _command_line;

  /// Syntax of the input file
  Syntax _syntax;

  /// OutputWarehouse object for this App
  OutputWarehouse _output_warehouse;

  /// The PerfGraph object for this application
  PerfGraph _perf_graph;

  /// The RankMap is a useful object for determining how the processes are laid out on the physical hardware
  const RankMap _rank_map;

  /// Input parameter storage structure (this is a raw pointer so the destruction time can be explicitly controlled)
  InputParameterWarehouse * _input_parameter_warehouse;

  /// The Factory responsible for building Actions
  ActionFactory _action_factory;

  /// Where built actions are stored
  ActionWarehouse _action_warehouse;

  /// Parser for parsing the input file
  Parser _parser;

  /// Pointer to the executioner of this run (typically build by actions)
  std::shared_ptr<Executioner> _executioner;

  /// Boolean to indicate whether to use a Nonlinear or EigenSystem (inspected by actions)
  bool _use_nonlinear;

  /// Boolean to indicate whether to use an eigenvalue executioner
  bool _use_eigen_value;

  /// System Information
  std::unique_ptr<SystemInfo> _sys_info;

  /// Indicates whether warnings, errors, or no output is displayed when unused parameters are detected
  enum UNUSED_CHECK
  {
    OFF,
    WARN_UNUSED,
    ERROR_UNUSED
  } _enable_unused_check;

  Factory _factory;

  /// Indicates whether warnings or errors are displayed when overridden parameters are detected
  bool _error_overridden;
  bool _ready_to_exit;

  /// This variable indicates when a request has been made to restart from an Exodus file
  bool _initial_from_file;

  /// The Exodus reader when _initial_from_file is set to true
  std::shared_ptr<ExodusII_IO> _ex_reader;

  /// This variable indicates that DistributedMesh should be used for the libMesh mesh underlying MooseMesh.
  bool _distributed_mesh_on_command_line;

  /// Whether or not this is a recovery run
  bool _recover;

  /// Whether or not this is a restart run
  bool _restart;

  /// Whether or not we are performing a split mesh operation (--split-mesh)
  bool _split_mesh;

  /// Whether or not we are using a (pre-)split mesh (automatically DistributedMesh)
  const bool _use_split;

  /// Whether or not FPE trapping should be turned on.
  bool _trap_fpe;

  /// The base name to restart/recover from.  If blank then we will find the newest checkpoint file.
  std::string _restart_recover_base;

  /// The file suffix to restart/recover from.  If blank then we will use the binary restart suffix.
  std::string _restart_recover_suffix;

  /// Whether or not this simulation should only run half its transient (useful for testing recovery)
  bool _half_transient;

  /// Map of outputer name and file number (used by MultiApps to propagate file numbers down through the multiapps)
  std::map<std::string, unsigned int> _output_file_numbers;

  /// true if we want to just check the input file
  bool _check_input;

  std::set<std::shared_ptr<RelationshipManager>> _relationship_managers;

  /// A map from undisplaced relationship managers to their displaced clone (stored as the base
  /// GhostingFunctor). Anytime we clone in attachRelationshipManagers we create a map entry from
  /// the cloned undisplaced relationship manager to its displaced clone counterpart. We leverage
  /// this map when removing relationship managers/ghosting functors
  std::unordered_map<RelationshipManager *, std::shared_ptr<GhostingFunctor>> _undisp_to_disp_rms;

  /// The library, registration method and the handle to the method
  std::map<std::pair<std::string, std::string>, void *> _lib_handles;

private:
  /** Method for creating the minimum required actions for an application (no input file)
   *
   * Mimics the following input file:
   *
   * [Mesh]
   *   type = GeneratedMesh
   *   dim = 1
   *   nx = 1
   * []
   *
   * [Executioner]
   *   type = Transient
   *   num_steps = 1
   *   dt = 1
   * []
   *
   * [Problem]
   *   solve = false
   * []
   *
   * [Outputs]
   *   console = false
   * []
   */
  void createMinimalApp();

  /**
   * Create the ordered mesh generators from all mesh generators
   */
  void createMeshGeneratorOrder();

  /**
   * @return whether we have created any clones for the provided template relationship manager and
   * mesh yet. This may be false for instance when we are in the initial add relationship manager
   * stage and haven't attempted attaching any relationship managers to the mesh or dof map yet
   * (which is when we generate the clones). It's also maybe possible that we've created a clone of
   * a given \p template_rm but not for the provided mesh so we return false in that case as well
   */
  bool hasRMClone(const RelationshipManager & template_rm, const MeshBase & mesh) const;

  /**
   * Return the relationship manager clone originally created from the provided template
   * relationship manager and mesh
   */
  RelationshipManager & getRMClone(const RelationshipManager & template_rm,
                                   const MeshBase & mesh) const;

  /**
   * Take an input relationship manager, clone it, and then initialize it with provided mesh and
   * optional \p dof_map
   * @param template_rm The relationship manager template from which we will clone
   * @param mesh The mesh to use for initialization
   * @param dof_map An optional parameter that, if provided, will be used to help init the cloned
   * relationship manager
   * @return a reference to the cloned and initialized relationship manager
   */
  RelationshipManager & createRMFromTemplateAndInit(const RelationshipManager & template_rm,
                                                    MeshBase & mesh,
                                                    const DofMap * dof_map = nullptr);

  /// Where the restartable data is held (indexed on tid)
  RestartableDataMaps _restartable_data;

  /// General storage for custom RestartableData that can be added to from outside applications
  std::unordered_map<RestartableDataMapName, std::pair<RestartableDataMap, std::string>>
      _restartable_meta_data;

  /**
   * Data names that will only be read from the restart file during RECOVERY.
   * e.g. these names are _excluded_ during restart.
   */
  DataNames _recoverable_data_names;

  /// Enumeration for holding the valid types of dynamic registrations allowed
  enum RegistrationType
  {
    APPLICATION,
    REGALL
  };

  /// The combined warehouse for storing any MooseObject based object
  std::unique_ptr<TheWarehouse> _the_warehouse;

  /// Level of multiapp, the master is level 0. This used by the Console to indent output
  unsigned int _multiapp_level;

  /// Numbering in all the sub-apps on the same level
  unsigned int _multiapp_number;

  /// The mesh from master app
  const MooseMesh * const _master_mesh;

  /// The displaced mesh from master app
  const MooseMesh * const _master_displaced_mesh;

  /// Holds the mesh generators until they have completed, then this structure is cleared
  std::map<std::string, std::shared_ptr<MeshGenerator>> _mesh_generators;

  /// Holds the ordered mesh generators until they have completed, then this structure is cleared
  std::vector<std::vector<std::shared_ptr<MeshGenerator>>> _ordered_generators;

  /// Holds the output for each mesh generator - including duplicates needed downstream
  std::map<std::string, std::list<std::unique_ptr<MeshBase>>> _mesh_generator_outputs;

  /// The final mesh generator name to use
  std::string _final_generator_name;

  /// The final Mesh that is generated by the generators
  std::list<std::unique_ptr<MeshBase> *> _final_generated_meshes;

  /// Cache for a Backup to use for restart / recovery
  std::shared_ptr<Backup> _cached_backup;

  /// Execution flags for this App
  ExecFlagEnum _execute_flags;

  /// Whether to turn on automatic scaling by default
  const bool _automatic_automatic_scaling;

  /// Whether the app is executing all mesh generators
  bool _executing_mesh_generators;

  /// Whether the mesh generator MeshBase has been popped off its storage container and is no
  /// longer accessible
  bool _popped_final_mesh_generator;

  /// CPU profiling
  bool _cpu_profiling = false;

  /// Memory profiling
  bool _heap_profiling = false;

  /// Map from a template relationship manager to a map in which the key-value pairs represent the \p
  /// MeshBase object and the clone of the template relationship manager, e.g. the top-level map key
  std::map<const RelationshipManager *,
           std::map<const MeshBase *, std::unique_ptr<RelationshipManager>>>
      _template_to_clones;

  // Allow FEProblemBase to set the recover/restart state, so make it a friend
  friend class FEProblemBase;
  friend class Restartable;
  friend class SubProblem;
};

template <typename T>
const T &
MooseApp::getParam(const std::string & name)
{
  return InputParameters::getParamHelper(name, _pars, static_cast<T *>(0));
}

template <typename T>
const T &
MooseApp::getParam(const std::string & name) const
{
  return InputParameters::getParamHelper(name, _pars, static_cast<T *>(0));
}
