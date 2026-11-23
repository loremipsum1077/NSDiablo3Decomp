#!/usr/bin/env python3
"""Interactive contributor onboarding; use --check for unattended diagnostics."""
import sys
sys.dont_write_bytecode = True
if sys.version_info < (3, 10):
    sys.exit("Python 3.10+ is required. Verify with python3 --version.")
from tools.onboarding import main
if __name__ == "__main__":
    raise SystemExit(main())
