# Profiling

# XPerf

XPerf is the preferred profiling tool.

# Code XL

AMDs Code XL is the second preferred profiling tool, since it properly picks up PDBs
for our hotreloaded DLLs.


# Very Sleepy CS

Very sleep does not properly pick up the PDBs for the hotreloaded DLLs. Only use
with hotreload disabled.

