#ifndef PRESSIO_IO_PLUGIN_H
#define PRESSIO_IO_PLUGIN_H
#include <string>
#include <memory>

/**
 * \file
 * \brief C++ interface to read and write files
 */

struct pressio_data;
struct pressio_options;

/**
 * plugin extension base class for io modules
 */
struct libpressio_io_plugin {
  public:

  virtual ~libpressio_io_plugin()=default;

  /** reads a pressio_data buffer from some persistent storage. Modules should override read_impl instead.
   * \param[in] data data object to populate, or nullptr to allocate it from
   * the file if supported.  callers should treat this buffer as if it is moved
   * in a C++11 sense
   *
   * \returns a new read pressio data buffer containing the read information
   * \see pressio_io_read for the semantics this function should obey
   */
  struct pressio_data* read(struct pressio_data* data);

  /** writes a pressio_data buffer to some persistent storage. Modules should override write_impl instead.
   * \param[in] data data to write
   * \see pressio_io_write for the semantics this function should obey
   */
  int write(struct pressio_data const* data);

  /** checks for extra arguments set for the io module. Modules should override check_options_impl instead.
   * the default verison just checks for unknown options passed in.
   *
   * \see pressio_io_check_options for semantics this function obeys
   * */
  int check_options(struct pressio_options const&);

  /** sets a set of options for the io_module 
   * \param[in] options to set for configuration of the io_module
   * \see pressio_io_set_options for the semantics this function should obey
   */
  int set_options(struct pressio_options const& options);
  /** get the compile time configuration of a io module. Modules should override get_configuration_impl instead
   *
   * \see pressio_io_get_configuration for the semantics this function should obey
   */
  struct pressio_options get_configuration() const;
  /** get a set of options available for the io module. Modules should override get_options_impl instead
   *
   * The io module should set a value if they have been set as default
   * The io module should set a "reset" value if they are required to be set, but don't have a meaningful default
   *
   * \see pressio_options.h for how to configure options
   */
  struct pressio_options get_options() const;

  /** get a implementation specific version string for the io module
   * \see pressio_io_version for the semantics this function should obey
   */
  virtual const char* version() const=0;
  /** get the major version, default version returns 0
   * \see pressio_io_major_version for the semantics this function should obey
   */
  virtual int major_version() const;
  /** get the minor version, default version returns 0
   * \see pressio_io_minor_version for the semantics this function should obey
   */
  virtual int minor_version() const;
  /** get the patch version, default version returns 0
   * \see pressio_io_patch_version for the semantics this function should obey
   */
  virtual int patch_version() const;
  /** get the error message for the last error
   * \returns an implementation specific c-style error message for the last error
   */
  const char* error_msg() const;
  /** get the error code for the last error
   * \returns an implementation specific integer error code for the last error, 0 is reserved for no error
   */
  int error_code() const;

  /**
   * clones an io module 
   * \returns a new reference to an io plugin.
   */
  virtual std::shared_ptr<libpressio_io_plugin> clone()=0;
  protected:
  /**
   * Should be used by implementing plug-ins to provide error codes
   * \param[in] code a implementation specific code for the last error
   * \param[in] msg a implementation specific message for the last error
   * \returns the value passed to code
   */
  int set_error(int code, std::string const& msg);

  /** checks for extra arguments set for the io module.
   * the default verison just checks for unknown options passed in.
   *
   * \see pressio_io_check_options for semantics this function obeys
   * */
  virtual int check_options_impl(struct pressio_options const&);

  /** reads a pressio_data buffer from some persistent storage
   * \param[in] data data object to populate, or nullptr to allocate it from the file if supported
   * \see pressio_io_read for the semantics this function should obey
   */
  virtual pressio_data* read_impl(struct pressio_data* data)=0;

  /** writes a pressio_data buffer to some persistent storage
   * \param[in] data data to write
   * \see pressio_io_write for the semantics this function should obey
   */
  virtual int write_impl(struct pressio_data const* data)=0;

  /** get the compile time configuration of a io module
   *
   * \see pressio_io_get_configuration for the semantics this function should obey
   */
  virtual struct pressio_options get_configuration_impl() const=0;

  /** sets a set of options for the io_module 
   * \param[in] options to set for configuration of the io_module
   * \see pressio_io_set_options for the semantics this function should obey
   */
  virtual int set_options_impl(struct pressio_options const& options)=0;

  /** get a set of options available for the io module
   *
   * The io module should set a value if they have been set as default
   * The io module should set a "reset" value if they are required to be set, but don't have a meaningful default
   *
   * \see pressio_options.h for how to configure options
   */
  virtual struct pressio_options get_options_impl() const=0;


  private:
  struct {
    int code;
    std::string msg;
  } error;
};

/**
 * wrapper for the io module to use in C
 */
struct pressio_io {
  /**
   * \param[in] impl a newly constructed io plugin
   */
  pressio_io(std::shared_ptr<libpressio_io_plugin>&& impl): plugin(std::forward<std::shared_ptr<libpressio_io_plugin>>(impl)) {}
  /**
   * defaults constructs a io with a nullptr
   */
  pressio_io()=default;
  /**
   * move constructs a io from another pointer
   */
  pressio_io(pressio_io&& io)=default;
  /**
   * move assigns a io from another pointer
   */
  pressio_io& operator=(pressio_io&& io)=default;

  /** \returns true if the plugin is set */
  operator bool() const {
    return bool(plugin);
  }

  /** make libpressio_io_plugin behave like a shared_ptr */
  libpressio_io_plugin& operator*() const noexcept {
    return *plugin;
  }

  /** make libpressio_io_plugin behave like a shared_ptr */
  libpressio_io_plugin* operator->() const noexcept {
    return plugin.get();
  }

  /**
   * pointer to the implementation
   */
  std::shared_ptr<libpressio_io_plugin> plugin;
};

#endif /* end of include guard: PRESSIO_IO_PLUGIN_H */
