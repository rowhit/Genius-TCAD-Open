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


// Local includes
#include "simulation_system.h"
#include "semiconductor_region.h"
#include "conductor_region.h"
#include "insulator_region.h"
#include "boundary_condition_neumann.h"


using PhysicalUnit::kb;
using PhysicalUnit::e;





///////////////////////////////////////////////////////////////////////
//----------------Function and Jacobian evaluate---------------------//
///////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------
 * build function and its jacobian for EBM3 solver
 */
void NeumannBC::EBM3_Function(PetscScalar * x, Vec f, InsertMode &add_value_flag)
{
  // Neumann boundary condition is processed here
  const PetscScalar Heat_Transfer = this->scalar("heat.transfer");

  // only consider heat exchange with external environment, if heat transfer rate is zero, do nothing
  if( Heat_Transfer == 0.0 ) return;

  // note, we will use ADD_VALUES to set values of vec f
  // if the previous operator is not ADD_VALUES, we should assembly the vec
  if( (add_value_flag != ADD_VALUES) && (add_value_flag != NOT_SET_VALUES) )
  {
    VecAssemblyBegin(f);
    VecAssemblyEnd(f);
  }


  BoundaryCondition::const_node_iterator node_it = nodes_begin();
  BoundaryCondition::const_node_iterator end_it = nodes_end();
  for(; node_it!=end_it; ++node_it )
  {
    // skip node not belongs to this processor
    if( (*node_it)->processor_id()!=Genius::processor_id() ) continue;


    const SimulationRegion * region = (*region_node_begin(*node_it)).second.first;
    const FVM_Node * fvm_node = (*region_node_begin(*node_it)).second.second;

    switch ( region->type() )
    {
    case SemiconductorRegion:
    case ElectrodeRegion:
    case InsulatorRegion:
      {

        unsigned int node_Tl_offset  = region->ebm_variable_offset(TEMPERATURE);

        if(region->get_advanced_model()->enable_Tl())
        {
          // process governing equation of T, which should consider heat exchange to environment
          PetscScalar T = x[fvm_node->local_offset()+node_Tl_offset];  // lattice temperature

          // add heat flux out of Neumann boundary to lattice temperature equatiuon
          PetscScalar S  = fvm_node->outside_boundary_surface_area();
          PetscScalar fT = Heat_Transfer*(T_external()-T)*S;

          VecSetValue(f, fvm_node->global_offset()+node_Tl_offset, fT, ADD_VALUES);
        }
        break;
      }
    case VacuumRegion:
      break;
    default: genius_error(); //we should never reach here
    }


  }

  // the last operator is ADD_VALUES
  add_value_flag = ADD_VALUES;
}








/*---------------------------------------------------------------------
 * build function and its jacobian for EBM3 solver
 */
void NeumannBC::EBM3_Jacobian(PetscScalar * x, SparseMatrix<PetscScalar> *jac, InsertMode &add_value_flag)
{
  // Neumann boundary condition is processed here
  const PetscScalar Heat_Transfer = this->scalar("heat.transfer");

  // only consider heat exchange with external environment, if heat transfer rate is zero, do nothing
  if( Heat_Transfer == 0.0 ) return;


  BoundaryCondition::const_node_iterator node_it = nodes_begin();
  BoundaryCondition::const_node_iterator end_it = nodes_end();
  for(; node_it!=end_it; ++node_it )
  {
    // skip node not belongs to this processor
    if( (*node_it)->processor_id()!=Genius::processor_id() ) continue;

    const SimulationRegion * region = (*region_node_begin(*node_it)).second.first;
    const FVM_Node * fvm_node = (*region_node_begin(*node_it)).second.second;

    switch ( region->type() )
    {
    case SemiconductorRegion:
    case ElectrodeRegion:
    case InsulatorRegion:
      {
        //the indepedent variable number, we only need 1 here.
        adtl::AutoDScalar::numdir=1;
        unsigned int node_Tl_offset  = region->ebm_variable_offset(TEMPERATURE);

        if(region->get_advanced_model()->enable_Tl())
        {
          // process governing equation of T, which should consider heat exchange to environment
          AutoDScalar T = x[fvm_node->local_offset()+node_Tl_offset];  T.setADValue(0, 1.0); // lattice temperature

          // add heat flux out of Neumann boundary to lattice temperature equatiuon
          PetscScalar S  = fvm_node->outside_boundary_surface_area();
          AutoDScalar fT = Heat_Transfer*(T_external()-T)*S;

          jac->add( fvm_node->global_offset()+node_Tl_offset,  fvm_node->global_offset()+node_Tl_offset,  fT.getADValue(0) );
        }

        break;
      }
    case VacuumRegion:
      break;
    default: genius_error(); //we should never reach here
    }


  }

  // the last operator is ADD_VALUES
  add_value_flag = ADD_VALUES;

}

