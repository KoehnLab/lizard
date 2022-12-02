# TODO list

- [ ] Find a way to make the implicit symmetrizations, GeCCo assumes, more explicit. For this, super-vertices need to be taken into account as those
  must not be symmetrized with one another.
- [ ] Find a way to cleverly split to-be-carried-out symmetrizations in a clever way (c.f. SR case: Only antisymmetrize one index-pair &
  column-symmetrize final result tensor only)
- [ ] Use approach given in https://stackoverflow.com/a/11327600 to perform common subexpression elimination (CSE). Also take
  https://doi.org/10.1007/11758501\_39 into account
