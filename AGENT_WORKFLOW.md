# Agent Workflow

Rules for automated/agent contributions to this repository.

## Branches
- Never commit directly to `master`.
- Use prefixed branches: `feature/<name>`, `fix/<name>`, `test/<name>`, `chore/<name>`.

## Commits
- [Conventional Commits](https://www.conventionalcommits.org/): `feat:`, `fix:`, `test:`, `chore:`, `refactor:`, `docs:`.
- Author commits as:
  ```
  git -c user.name="Claude" -c user.email="noreply@anthropic.com" commit -m "..."
  ```

## Pull Requests
- One reviewable unit per PR; open against `master`.
- The PR body must contain `Closes #<nr>` for every issue it addresses.
- Add Copilot as a reviewer.
- All CI checks must be green before merge, and no open review comment may remain unresolved.

## Architecture invariants
- `NanoESP_HTTP` and `NanoESP_MQTT` aggregate `NanoESP` by reference (no inheritance).
- Do not introduce new global state in `.cpp` files — keep state in class
  members (some legacy file-scope globals still exist and should be migrated).
- Target ATmega328 (2 KB RAM): be frugal with `String` objects and VLAs.
- `test/` code must not include real Arduino headers; it must compile in the PlatformIO `native` environment.

## Testing
- Examples are compiled in CI with `arduino-cli` for `arduino:avr:nano`.
- Native unit tests run with `pio test -e native` (see `platformio.ini` and `test/`).
