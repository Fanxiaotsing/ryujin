//
// SPDX-License-Identifier: MIT
// Copyright (C) 2020 by the ryujin authors
//

#ifndef PROBLEM_DESCRIPTION_TEMPLATE_H
#define PROBLEM_DESCRIPTION_TEMPLATE_H

#include "problem_description.h"

namespace ryujin
{
  using namespace dealii;


  ProblemDescription::ProblemDescription(
      const std::string &subsection /*= "ProblemDescription"*/)
      : ParameterAcceptor(subsection)
  {
    ParameterAcceptor::parse_parameters_call_back.connect(
        std::bind(&ProblemDescription::parse_parameters_callback, this));

    description_ = "Euler";
    add_parameter(
        "Description",
        description_,
        "Description - valid options are \"Euler\" and \"Navier Stokes\"");

    gamma_ = 7. / 5.;
    add_parameter("gamma", gamma_, "Euler: Ratio of specific heats");

    b_ = 0.;
    add_parameter("b", b_, "Euler: Covolume");

    mu_ = 1.e-3;
    add_parameter("mu", mu_, "Navier Stokes: Shear viscosity");

    lambda_ = 0.;
    add_parameter("lambda", lambda_, "Navier Stokes: Bulk viscosity");

    cv_inverse_kappa_ = 1.866666666666666e-2;
    add_parameter("kappa", cv_inverse_kappa_, "Navier Stokes: Scaled thermal conductivity c_v^{-1} kappa");
  }


  void ProblemDescription::parse_parameters_callback()
  {
    /*
     * Precompute a number of derived gamma coefficients that contain
     * divisions:
     */
    gamma_inverse_ = 1. / gamma_;
    gamma_plus_one_inverse_ = 1. / (gamma_ + 1.);

    static_assert(covolume_ == false, "not implemented");
    if constexpr (!covolume_)
      AssertThrow(b_ == 0.,
                  dealii::ExcMessage(
                      "ProblemDescription::covolume_ compile-time option set "
                      "to false but nonzero co-volume prescribed."));
  }


#ifndef DOXYGEN
  template <>
  const std::array<std::string, 3> ProblemDescription::component_names<1>{
      {"rho", "m", "E"}};

  template <>
  const std::array<std::string, 4> ProblemDescription::component_names<2>{
      {"rho", "m_1", "m_2", "E"}};

  template <>
  const std::array<std::string, 5> ProblemDescription::component_names<3>{
      {"rho", "m_1", "m_2", "m_3", "E"}};
#endif

} /* namespace ryujin */

#endif /* PROBLEM_DESCRIPTION_TEMPLATE_H */
