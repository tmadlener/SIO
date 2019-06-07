#pragma once

// -- sio headers
#include <sio/definitions.h>
#include <sio/buffer.h>

namespace sio {
  
  class buffer ;
  class buffer_span ;

  /**
   *  @brief  read_device class.
   *
   *  Holds a buffer_span and a cursor allowing to read 
   *  data sequentially, moving the cursor after reading.
   */
  class read_device {
  public:
    using cursor_type = std::size_t ;
    using size_type = std::size_t ;

  public:
    /// Default constructor
    read_device() = default ;
    /// Default copy constructor
    read_device( const read_device & ) = default ;
    /// Default move constructor
    read_device( read_device && ) = default ;
    /// Default assignement operator
    read_device& operator=( const read_device & ) = default ;
    /// Default move assignement operator
    read_device& operator=( read_device && ) = default ;
    /// Default destructor
    ~read_device() = default ;
    
    /**
     *  @brief  Constructor with buffer span
     * 
     *  @param  buf the input buffer span
     */
    read_device( buffer_span buf ) ;
    
    /**
     *  @name Buffer
     */
    ///{@
    /**
     *  @brief  Set the buffer span to use (move)
     * 
     *  @param  buf the buffer to use
     */
    void set_buffer( buffer_span &&buf ) ;
    
    /**
     *  @brief  Set the buffer span to use (copy)
     * 
     *  @param  buf the buffer to use
     */
    void set_buffer( const buffer_span &buf ) ;
    ///@}

    /**
     *  @name Cursor
     */
    ///{@
    /**
     *  @brief  Get the cursor position
     */
    cursor_type position() const ;
    
    /**
     *  @brief  Seek the cursor at a given position
     * 
     *  @param  pos the new cursor position
     */
    void seek( cursor_type pos ) ;
    ///@}

    /**
     *  @name I/O operations
     */
    ///{@
    /**
     *  @brief  Read out a variable from the buffer. Move the cursor accordingly
     * 
     *  @param  var the variable to receive
     */
    template <typename T>
    void data( T &var ) ;

    /**
     *  @brief  Read out an array of variables from the buffer. Move the cursor accordingly
     * 
     *  @param  var the address of the array
     *  @param  count the number of element to read out
     */
    template <typename T>
    void data( T *var, size_type count ) ;
    
    /**
     *  @brief  Read out a "pointer to" pointer from the buffer.
     *          A new entry is created for a future relocation.
     *          The pointer relocation is performed at the end of record reading
     *          
     *  @param  ptr the address to register
     */
    void pointer_to( ptr_type *ptr ) ;
    
    /**
     *  @brief  Read out a "pointed at" pointer from the buffer.
     *          A new entry is created for a future relocation.
     *          The pointer relocation is performed at the end of record reading
     *          
     *  @param  ptr the address to register
     */
    void pointed_at( ptr_type *ptr ) ;
    
    /**
     *  @brief  Perform the pointer relocation after the whole record has
     *          been read. The pointers are relocated and the pointer maps
     *          are cleared
     */
    void pointer_relocation() ;
    ///@}

  private:
    ///< The buffer span
    buffer_span         _buffer {} ;
    ///< The device cursor
    cursor_type         _cursor {0} ;
    ///< The map of pointer "pointed at"
    pointed_at_map      _pointed_at {} ;
    ///< The map of pointer "pointer to"
    pointer_to_map      _pointer_to {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  write_device class.
   *
   *  Holds a buffer and provide a api to write data sequentially 
   *  to this buffer by using a cursor
   */
  class write_device {
  public:
    using cursor_type = std::size_t ;
    using size_type = std::size_t ;

  public:
    /// Default constructor
    write_device() = default ;
    /// Default copy constructor
    write_device( const write_device & ) = default ;
    /// Default move constructor
    write_device( write_device && ) = default ;
    /// Default assignement operator
    write_device& operator=( const write_device & ) = default ;
    /// Default move assignement operator
    write_device& operator=( write_device && ) = default ;
    /// Default destructor
    ~write_device() = default ;
    
    /**
     *  @name Buffer
     */
    ///{@
    /**
     *  @brief  Set the buffer to use (move)
     * 
     *  @param  buf the buffer to use
     */
    void set_buffer( buffer&& buf ) ;
    
    /**
     *  @brief  Take out the buffer from the device.
     *          The buffer in the device becomes invalid
     */
    buffer take_buffer() ;
    ///@}
    
    /**
     *  @name Cursor
     */
    ///{@
    /**
     *  @brief  Get the cursor position
     */
    cursor_type position() const ;
    
    /**
     *  @brief  Seek the cursor at a given position
     * 
     *  @param  pos the new cursor position
     */
    void seek( cursor_type pos ) ;
    ///@}

    /**
     *  @name I/O operations
     */
    ///{@
    /**
     *  @brief  Write out a variable to the buffer. Move the cursor accordingly
     * 
     *  @param  var the variable to write
     */
    template <typename T>
    void data( const T &var ) ;

    /**
     *  @brief  Write out an array of variables to the buffer. Move the cursor accordingly
     * 
     *  @param  var the address of the array
     *  @param  count the number of element to write out
     */
    template <typename T>
    void data( const T *const var, size_type count ) ;
    
    /**
     *  @brief  Write out a "pointer to" pointer to the buffer.
     *          A new entry is created for a future relocation.
     *          The pointer relocation is performed at the end of record writting
     *          
     *  @param  ptr the address to register
     */
    void pointer_to( ptr_type *ptr ) ;
    
    /**
     *  @brief  Write out a "pointed at" pointer to the buffer.
     *          A new entry is created for a future relocation.
     *          The pointer relocation is performed at the end of record writting
     *          
     *  @param  ptr the address to register
     */
    void pointed_at( ptr_type *ptr ) ;
    
    /**
     *  @brief  Perform the pointer relocation after the whole record has
     *          been written. The pointers are relocated and the pointer maps
     *          are cleared
     */
    void pointer_relocation() ;
    ///@}
    
  private:
    ///< The buffer in which to write
    buffer              _buffer {32*sio::kbyte*sio::kbyte} ;
    ///< The device cursor
    cursor_type         _cursor {0} ;
    ///< The map of pointer "pointed at"
    pointed_at_map      _pointed_at {} ;
    ///< The map of pointer "pointer to"
    pointer_to_map      _pointer_to {} ;
  };

}

#include <sio/api.h>

namespace sio {

  template <typename T>
  inline void read_device::data( T &var ) {
    data( &var, 1 ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void read_device::data( T *var, size_type count ) {
    _cursor += sio::api::read( _buffer, var, _cursor, count ) ;
  }
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename T>
  inline void write_device::data( const T &var ) {
    data( &var, 1 ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void write_device::data( const T *const var, size_type count ) {
    _cursor += sio::api::write( _buffer, var, _cursor, count ) ;
  }

}