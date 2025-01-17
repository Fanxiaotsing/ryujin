//
// SPDX-License-Identifier: MIT
// Copyright (C) 2020 - 2022 by the ryujin authors
//

#pragma once

#include "hyperbolic_system.h"
#include <initial_state.h>

namespace ryujin
{
  namespace ShallowWater
  {
    /**
     * Ritter's dam break solution. This is one-dimensional dam break without
     * friction. See: Sec.~7.3 in @cite GuermondEtAl2018SW.
     *
     * @ingroup ShallowWaterEquations
     */
    template <int dim, typename Number, typename state_type>
    class RitterDamBreak : public InitialState<dim, Number, state_type, 1>
    {
    public:
      RitterDamBreak(const HyperbolicSystem &hyperbolic_system,
                     const std::string subsection)
          : InitialState<dim, Number, state_type, 1>("ritter dam break",
                                                     subsection)
          , hyperbolic_system(hyperbolic_system)
      {
        dealii::ParameterAcceptor::parse_parameters_call_back.connect(
            std::bind(&RitterDamBreak::parse_parameters_callback, this));

        t_initial_ = 0.1;
        this->add_parameter("time initial",
                            t_initial_,
                            "Time at which initial state is prescribed");

        left_depth = 0.005;
        this->add_parameter("left water depth",
                            left_depth,
                            "Depth of water to the left of pseudo-dam (x<0)");
      }

      void parse_parameters_callback()
      {
        AssertThrow(t_initial_ > 0.,
                    dealii::ExcMessage("Expansion must be computed at an "
                                       "initial time greater than 0."));
      }

      state_type compute(const dealii::Point<dim> &point, Number t) final
      {
        const auto g = hyperbolic_system.gravity();
        const Number x = point[0];

        const Number aL = std::sqrt(g * left_depth);
        const Number xA = -(t + t_initial_) * aL;
        const Number xB = Number(2.) * (t + t_initial_) * aL;

        const Number tmp = aL - x / (2. * (t + t_initial_));

        const Number h_expansion = 4. / (9. * g) * tmp * tmp;
        const Number v_expansion = 2. / 3. * (x / (t + t_initial_) + aL);

        if (x <= xA)
          return hyperbolic_system.template expand_state<dim>(
              HyperbolicSystem::state_type<1, Number>{
                  {left_depth, Number(0.)}});
        else if (x <= xB)
          return hyperbolic_system.template expand_state<dim>(
              HyperbolicSystem::state_type<1, Number>{
                  {h_expansion, h_expansion * v_expansion}});
        else
          return hyperbolic_system.template expand_state<dim>(
              HyperbolicSystem::state_type<1, Number>{
                  {Number(0.), Number(0.)}});
      }

      /* Default bathymetry of 0 */

    private:
      const HyperbolicSystem &hyperbolic_system;

      Number t_initial_;

      Number left_depth;
    };

  } // namespace ShallowWater
} // namespace ryujin
