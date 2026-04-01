# Windows OWL Daemon Port Plan

## Goal

Build a runnable `owl.exe` daemon on Windows that provides behavior equivalent to the current Linux/macOS daemon:

- capture and inject AWDL frames
- maintain AWDL peer/election/sync state using the existing portable core (`src/`)
- expose a virtual interface for IPv6 traffic bridging
- run as a long-lived process with logging and lifecycle control

This document defines an implementation roadmap from the current state (Windows supports core library + tests only) to a production-capable daemon.

---

## Current status

What already works:

- `src/` core AWDL logic compiles on Windows.
- unit tests build and run on Windows CI.

What is not implemented on Windows:

- daemon executable target (`daemon/owl.c` + `daemon/core.c` + `daemon/io.c` + `daemon/netutils.c`)
- platform integration for Wi-Fi monitor/injection and virtual interface plumbing

---

## Key constraints on Windows

1. **Monitor/injection capability is hardware/driver-dependent**
   - Unlike Linux/macOS, Windows APIs do not consistently expose monitor + injection in a unified way.
   - Practical implementation usually depends on **Npcap** and adapter/driver support.

2. **Virtual interface support requires a driver**
   - Linux/macOS tun/tap paths in `daemon/io.c` are not portable.
   - Recommended options: **Wintun** (preferred) or TAP driver.

3. **Channel control APIs differ**
   - Existing netlink/CoreWLAN logic is not reusable.
   - Channel management may be limited or adapter-specific on Windows.

---

## Target architecture

Introduce a strict platform abstraction for daemon integration points:

- `daemon/platform/platform.h`
  - platform-agnostic daemon contracts
- `daemon/platform/common/*`
  - shared helper code
- `daemon/platform/linux/*`
  - move Linux-specific code from current `io.c` / `netutils.c`
- `daemon/platform/macos/*`
  - move macOS-specific code from current `io.c` / `corewlan.m` / `netutils.c`
- `daemon/platform/windows/*`
  - new Npcap + Wintun implementation

Core daemon flow (`owl.c`, scheduler logic, AWDL state machine orchestration) should call only the platform interface.

---

## Windows dependency plan

### Required

- **Npcap SDK** (packet capture/injection)
- **Wintun SDK** (virtual L3 interface)

### Optional / future

- Windows service wrapper (SCM integration)
- ETW logging provider

---

## Milestones

## M1 - Refactor for platform interface (no behavior change)

Deliverables:

- Extract platform contracts from existing daemon code:
  - frame RX/TX backend
  - virtual interface RX/TX backend
  - peer/neighbor integration hooks
  - channel switching hook
- Keep Linux/macOS working through adapters that implement the new interface.

Exit criteria:

- Existing Linux/macOS daemon builds and behaves unchanged.
- No Windows implementation yet; just abstraction layer.

---

## M2 - Windows compile target with stubs

Deliverables:

- Add `daemon/platform/windows/` with compile-safe stub implementation.
- Add `owl.exe` Windows build target behind option:
  - `OWL_BUILD_WINDOWS_DAEMON_EXPERIMENTAL`
- Runtime prints clear "feature not implemented" for stubbed capabilities.

Exit criteria:

- Windows daemon target compiles in CI.
- Binary starts and exits gracefully with actionable diagnostics.

---

## M3 - RX/TX over Npcap

Deliverables:

- Enumerate interfaces via Npcap.
- Open selected adapter in mode required for AWDL frame handling.
- Implement:
  - receive radiotap + 802.11 frames
  - inject crafted frames

Exit criteria:

- End-to-end action/data frame loop works against controlled test setup on supported adapter.

Risks:

- adapter/driver may not support required monitor/injection path.
- fallback mode may need receive-only support first.

---

## M4 - Virtual interface via Wintun

Deliverables:

- Create/manage Wintun interface (`awdl0`-equivalent naming policy on Windows).
- Bridge packets:
  - AWDL data frame -> Wintun packet
  - Wintun packet -> AWDL data frame

Exit criteria:

- IPv6 traffic can traverse AWDL<->Wintun path in lab tests.

---

## M5 - Channel + peer integration on Windows

Deliverables:

- Windows-specific implementation for:
  - channel selection strategy (best effort by adapter support)
  - neighbor/route behavior equivalent to Linux/macOS design intent

Exit criteria:

- Stable peer discovery and packet exchange on supported hardware.

---

## M6 - Production hardening

Deliverables:

- adapter capability checks at startup (clear fail messages)
- robust reconnect and recovery paths
- structured logs and diagnostics bundle
- CI split:
  - build-only on hosted Windows
  - hardware-in-loop smoke tests on self-hosted runners

Exit criteria:

- repeatable startup/run/shutdown behavior
- documented supported adapter matrix

---

## Immediate implementation checklist (next coding steps)

1. Create platform interface headers and migrate current Linux/macOS code behind them.
2. Introduce `daemon/platform/windows/` with stubs and compile target option.
3. Add explicit startup diagnostics for unsupported adapter features.
4. Add integration test harness that can replay capture files through the daemon pipeline.

---

## Suggested acceptance tests

- Build checks:
  - Linux/macOS daemon still builds.
  - Windows experimental daemon builds.
- Functional checks:
  - daemon starts, selects interface, initializes platform backend.
  - controlled frame replay exercises `awdl_rx`/`awdl_tx` paths through daemon pipeline.
  - Wintun packet loopback path works.
- Stability checks:
  - repeated start/stop cycles without leaked handles.

---

## Notes

- Full Windows parity depends on Wi-Fi adapter/driver capabilities.
- A receive-only intermediate mode may be needed before full injection support.
- The plan intentionally keeps AWDL protocol logic in `src/` untouched and focuses on daemon platform integration boundaries.
