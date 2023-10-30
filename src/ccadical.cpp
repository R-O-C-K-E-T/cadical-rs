#include "../cadical/src/ccadical.cpp"

#include <iostream>

struct CExternalPropagator: public ExternalPropagator {
  void *state;

  // All non-null
  void (*notify_assignment_callback) (void* state, int lit, bool is_fixed);
  void (*notify_new_decision_level_callback) (void* state);
  void (*notify_backtrack_callback) (void* state, size_t new_level);
  int (*cb_propagate_callback) (void* state);
  int (*cb_add_reason_clause_lit_callback) (void* state, int propagated_lit);

  int cb_propagate() override {
    return cb_propagate_callback(state);
  }

  // int cb_add_external_clause_lit() override {
  //   // TODO
  // }

  void notify_assignment(int lit, bool is_fixed) override {
    notify_assignment_callback(state, lit, is_fixed);
  }

  void notify_new_decision_level() override {
    notify_new_decision_level_callback(state);
  }

  void notify_backtrack(size_t new_level) override {
    notify_backtrack_callback(state, new_level);
  }

  bool cb_check_found_model(const std::vector<int> &) override {
    return true;
  }

  bool cb_has_external_clause () override {
    return false;
    // TODO
    // return external_propagator.external_clauses.size() > 0;
  }

  int cb_add_external_clause_lit () override {
    return 0;
    // TODO
    // int ret = external_propagator.external_clauses.front();
    // external_propagator.external_clauses.pop();
    // return ret;
  };

};

extern "C"
{

  int ccadical_status(CCaDiCaL *wrapper)
  {
    return ((Wrapper *)wrapper)->solver->status();
  }

  int ccadical_vars(CCaDiCaL *wrapper)
  {
    return ((Wrapper *)wrapper)->solver->vars();
  }

  const char *ccadical_read_dimacs(CCaDiCaL *wrapper, const char *path,
                                   int &vars, int strict)
  {
    return ((Wrapper *)wrapper)->solver->read_dimacs(path, vars, strict);
  }

  const char *ccadical_write_dimacs(CCaDiCaL *wrapper, const char *path,
                                    int min_max_var = 0)
  {
    return ((Wrapper *)wrapper)->solver->write_dimacs(path, min_max_var);
  }

  int ccadical_configure(CCaDiCaL *wrapper, const char *name)
  {
    return ((Wrapper *)wrapper)->solver->configure(name);
  }

  int ccadical_limit2(CCaDiCaL *wrapper,
                      const char *name, int val)
  {
    return ((Wrapper *)wrapper)->solver->limit(name, val);
  }

ExternalPropagator* ccadical_connect_external_propagator(
  CCaDiCaL *wrapper, 
  void *state,
  void (*notify_assignment) (void* state, int lit, bool is_fixed),
  void (*notify_new_decision_level) (void* state),
  void (*notify_backtrack) (void* state, size_t new_level),
  int (*cb_propagate) (void* state),
  int (*cb_add_reason_clause_lit) (void* state, int propagated_lit)
) {
  
  auto propagator = new CExternalPropagator();
  propagator->state = state;
  propagator->notify_assignment_callback = notify_assignment;
  propagator->notify_new_decision_level_callback = notify_new_decision_level; 
  propagator->notify_backtrack_callback = notify_backtrack;
  propagator->cb_propagate_callback = cb_propagate;
  propagator->cb_add_reason_clause_lit_callback = cb_add_reason_clause_lit;

  ((Wrapper *)wrapper)->solver->connect_external_propagator(propagator);

  return propagator;
}

void ccadical_disconnect_external_propagator(CCaDiCaL *ptr, ExternalPropagator* propagator) {
  Wrapper *wrapper = (Wrapper *) ptr;

  wrapper->solver->disconnect_external_propagator();
  delete propagator;
}

void ccadical_add_observed_var(CCaDiCaL *ptr, int lit) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->solver->add_observed_var(lit);
}

void ccadical_remove_observed_var(CCaDiCaL *ptr, int lit) {
  Wrapper *wrapper = (Wrapper *) ptr;
  wrapper->solver->remove_observed_var(lit);
}
}
