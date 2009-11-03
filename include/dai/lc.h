/*  This file is part of libDAI - http://www.libdai.org/
 *
 *  libDAI is licensed under the terms of the GNU General Public License version
 *  2, or (at your option) any later version. libDAI is distributed without any
 *  warranty. See the file COPYING for more details.
 *
 *  Copyright (C) 2006-2009  Joris Mooij  [joris dot mooij at libdai dot org]
 *  Copyright (C) 2006-2007  Radboud University Nijmegen, The Netherlands
 */


/// \file
/// \brief Defines class LC
/// \todo Improve documentation


#ifndef __defined_libdai_lc_h
#define __defined_libdai_lc_h


#include <string>
#include <dai/daialg.h>
#include <dai/enum.h>
#include <dai/factorgraph.h>
#include <dai/properties.h>
#include <dai/exceptions.h>


namespace dai {


/// Approximate inference algorithm "Loop Corrected Belief Propagation" by Mooij and Kappen
class LC : public DAIAlgFG {
    private:
        std::vector<Factor>      _pancakes;      // used by all LC types (psi_I is stored in the pancake)
        std::vector<Factor>      _cavitydists;   // used by all LC types to store the approximate cavity distribution
        /// _phis[i][_I] corresponds to \f$ \phi^{\setminus i}_I(x_{I \setminus i}) \f$
        std::vector<std::vector<Factor> >      _phis;

        /// Single variable beliefs
        std::vector<Factor>      _beliefs;

        /// Maximum difference encountered so far
        Real                    _maxdiff;
        /// Number of iterations needed
        size_t                  _iters;

    public:
        /// Parameters of this inference algorithm
        struct Properties {
            /// Enumeration of possible ways to initialize the cavities
            DAI_ENUM(CavityType,FULL,PAIR,PAIR2,UNIFORM);

            /// Enumeration of different update schedules
            DAI_ENUM(UpdateType,SEQFIX,SEQRND,NONE);

            /// Verbosity
            size_t verbose;

            /// Maximum number of iterations
            size_t maxiter;

            /// Tolerance
            Real tol;

            /// Complete or partial reinit of cavity graphs?
            bool reinit;

            /// Damping constant
            Real damping;

            /// How to initialize the cavities
            CavityType cavity;

            /// What update schedule to use
            UpdateType updates;

            /// Name of the algorithm used to initialize the cavity distributions
            std::string cavainame;      // FIXME: needs assignment operator?

            /// Parameters for the algorithm used to initialize the cavity distributions
            PropertySet cavaiopts;      // FIXME: needs assignment operator?
        } props;

        /// Name of this inference algorithm
        static const char *Name;

    public:
        /// Default constructor
        LC() : DAIAlgFG(), _pancakes(), _cavitydists(), _phis(), _beliefs(), _maxdiff(), _iters(), props() {}

        /// Construct from FactorGraph fg and PropertySet opts
        LC( const FactorGraph &fg, const PropertySet &opts );


    /// \name General InfAlg interface
    //@{
        virtual LC* clone() const { return new LC(*this); }
        virtual std::string identify() const;
        virtual Factor belief( const Var &n ) const { return( _beliefs[findVar(n)] ); }
        virtual Factor belief( const VarSet &/*ns*/ ) const { DAI_THROW(NOT_IMPLEMENTED); return Factor(); }
        virtual Factor beliefV( size_t i ) const { return _beliefs[i]; }
        virtual std::vector<Factor> beliefs() const { return _beliefs; }
        virtual Real logZ() const { DAI_THROW(NOT_IMPLEMENTED); return 0.0; }
        virtual void init();
        virtual void init( const VarSet &/*ns*/ ) { init(); }
        virtual Real run();
        virtual Real maxDiff() const { return _maxdiff; }
        virtual size_t Iterations() const { return _iters; }
        virtual void setProperties( const PropertySet &opts );
        virtual PropertySet getProperties() const;
        virtual std::string printProperties() const;
    //@}

    /// \name Additional interface specific for LC
    //@{
        Real CalcCavityDist( size_t i, const std::string &name, const PropertySet &opts );
        Real InitCavityDists( const std::string &name, const PropertySet &opts );
        long SetCavityDists( std::vector<Factor> &Q );

        Factor NewPancake (size_t i, size_t _I, bool & hasNaNs);

        void CalcBelief (size_t i);
        const Factor &belief (size_t i) const { return _beliefs[i]; };
        const Factor &pancake (size_t i) const { return _pancakes[i]; };
        const Factor &cavitydist (size_t i) const { return _cavitydists[i]; };
    //@}
};


} // end of namespace dai


#endif
