//
// SPDX-License-Identifier: MIT
// Copyright (C) 2020 by the ryujin authors
//

#ifndef TIME_LOOP_H
#define TIME_LOOP_H

#include <compile_time_options.h>

#include "discretization.h"
#include "initial_values.h"
#include "offline_data.h"
#include "postprocessor.h"
#include "time_step.h"

#include <deal.II/base/parameter_acceptor.h>
#include <deal.II/base/timer.h>

#include <fstream>
#include <sstream>
#include <future>

namespace ryujin
{

  template <int dim, typename Number = double>
  class TimeLoop final : public dealii::ParameterAcceptor
  {
  public:
    using scalar_type = typename OfflineData<dim, Number>::scalar_type;
    using vector_type = typename OfflineData<dim, Number>::vector_type;

    TimeLoop(const MPI_Comm &mpi_comm);

    void run();

  private:
    /* Private methods for run(): */

    void initialize();

    void compute_error(const vector_type &U, Number t);

    void output(const vector_type &U,
                const std::string &name,
                Number t,
                unsigned int cycle);

    void print_parameters(std::ostream &stream);
    void print_mpi_partition(std::ostream &stream);
    void print_memory_statistics(std::ostream &stream);
    void print_timers(std::ostream &stream);
    void print_throughput(unsigned int cycle, Number t, std::ostream &stream);

    void print_info(const std::string &header);
    void print_head(const std::string &header,
                    const std::string &secondary,
                    std::ostream &stream);

    void print_cycle_statistics(unsigned int cycle,
                                Number t,
                                unsigned int output_cycle,
                                bool write_to_logfile = false,
                                bool final_time = false);

    /* Options: */

    std::string base_name;

    Number t_initial;
    Number t_final;
    Number output_granularity;

    bool enable_checkpointing;
    bool enable_output_full;
    bool enable_output_cutplanes;
    bool enable_compute_error;

    unsigned int output_checkpoint_multiplier;
    unsigned int output_full_multiplier;
    unsigned int output_cutplanes_multiplier;

    bool resume;

    unsigned int terminal_update_interval;

    /* Data: */

    const MPI_Comm &mpi_communicator;

    std::map<std::string, dealii::Timer> computing_timer;

    ryujin::Discretization<dim> discretization;
    ryujin::OfflineData<dim, Number> offline_data;
    ryujin::InitialValues<dim, Number> initial_values;
    ryujin::TimeStep<dim, Number> time_step;
    ryujin::Postprocessor<dim, Number> postprocessor;

    const unsigned int mpi_rank;
    const unsigned int n_mpi_processes;

    std::ofstream logfile; /* log file */
  };

} // namespace ryujin

#endif /* TIME_LOOP_H */