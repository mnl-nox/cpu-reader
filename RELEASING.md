# Release process

## Versioning

Use Semantic Versioning with a leading `v`:

- `vMAJOR.MINOR.PATCH` for stable releases.
- `vMAJOR.MINOR.PATCH-alpha.N`, `-beta.N`, or `-rc.N` for pre-releases.
- Increment MAJOR for incompatible public API changes, MINOR for backward-compatible features, and PATCH for backward-compatible fixes.

Do not move or overwrite an existing version tag. Every tag must point to the exact commit that was validated.

## Required checks

Before tagging a release, run locally:

```sh
make clean
make test
make test-sanitize
make test-security
cppcheck --enable=warning,style,performance,portability --error-exitcode=1 --inline-suppr --std=c99 -Iinclude src tests examples bench
```

The GitHub Actions release workflow repeats the functional tests, sanitizers, hardening build, and static analysis. A GitHub Release is published only after those checks pass for a pushed `v*.*.*` tag.

## Release procedure

1. Update the version/status references and release notes.
2. Ensure the change is reviewed and merged to `main`.
3. Create an annotated tag from the validated commit, for example:
   `git tag -a v1.0.0-beta.2 -m "CPU Reader v1.0.0-beta.2"`
4. Push the tag: `git push origin v1.0.0-beta.2`.
5. Confirm the **Release** workflow succeeds and inspect the generated GitHub Release notes.
6. If any required check fails, do not publish or advertise the version as validated. Fix the branch, rerun CI, and create a new version tag only after the correction is merged.

Do not create a release tag from an unreviewed feature branch. Tags are release artifacts, not a substitute for pull-request review.
