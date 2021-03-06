/********************************************************************************/
/*     888888    888888888   88     888  88888   888      888    88888888       */
/*   8       8   8           8 8     8     8      8        8    8               */
/*  8            8           8  8    8     8      8        8    8               */
/*  8            888888888   8   8   8     8      8        8     8888888        */
/*  8      8888  8           8    8  8     8      8        8            8       */
/*   8       8   8           8     8 8     8      8        8            8       */
/*     888888    888888888  888     88   88888     88888888     88888888        */
/*                                                                              */
/*       A Three-Dimensional General Purpose Semiconductor Simulator.           */
/*                                                                              */
/*                                                                              */
/*  Copyright (C) 2007-2008                                                     */
/*  Cogenda Pte Ltd                                                             */
/*                                                                              */
/*  Please contact Cogenda Pte Ltd for license information                      */
/*                                                                              */
/*  Author: Gong Ding   gdiso@ustc.edu                                          */
/*                                                                              */
/********************************************************************************/



#ifndef __fvm_cell_data_semiconductor_h__
#define __fvm_cell_data_semiconductor_h__


#include "fvm_cell_data.h"


/**
 *  FVM cell data for semiconductor region
 */
class FVM_Semiconductor_CellData : public FVM_CellData
{

public:

  /**
   * the vector auxiliary variable for semiconductor region
   */
  enum SemiconductorAuxVecData
  {
    /**
     * electrical field
     */
    _E_=0,

    /**
     * electron current field
     */
    _Jn_,

    /**
     * electron current field
     */
    _Jp_,

    /**
     * last enum number
     */
    VectorDataCount
  };



public:
  /**
   * constructor
   */
  FVM_Semiconductor_CellData(DataStorage * data_storage, const std::map<std::string, SimulationVariable> & variables)
  :FVM_CellData(data_storage, variables)
  {}

  /**
   * destructor
   */
  virtual ~FVM_Semiconductor_CellData()  {}

public:

  /**
   * @return the solution variable number
   */
  static size_t n_scalar()
  { return 0;  }

  /**
   * @return the complex variable number
   */
  static size_t n_complex()
  { return 0; }

  /**
   * @return the vector variable number
   */
  static size_t n_vector()
  { return static_cast<unsigned int>(VectorDataCount); }

  /**
   * @return the tensor variable number
   */
  static size_t n_tensor()
  { return 0; }

  /**
   * @return the data type
   */
  virtual CellDataType type() const
    { return FVM_CellData::SemiconductorData; }


public:

  /**
   * @return the electrical field
   */
  virtual VectorValue<PetscScalar> E()       const
  { return _data_storage->vector(_E_, _offset);}


  /**
   * @return the writable reference to electrical field
   */
  virtual VectorValue<PetscScalar> & E()
  { return _data_storage->vector(_E_, _offset);}


  /**
   * @return the electron current
   */
  virtual VectorValue<PetscScalar> Jn()       const
  { return _data_storage->vector(_Jn_, _offset);}


  /**
   * @return the writable reference to electron current
   */
  virtual VectorValue<PetscScalar> & Jn()
  { return _data_storage->vector(_Jn_, _offset);}


  /**
   * @return the hole current
   */
  virtual VectorValue<PetscScalar> Jp()       const
  { return _data_storage->vector(_Jp_, _offset);}


  /**
   * @return the writable reference to hole current
   */
  virtual VectorValue<PetscScalar> & Jp()
  { return _data_storage->vector(_Jp_, _offset);}



};




#endif
