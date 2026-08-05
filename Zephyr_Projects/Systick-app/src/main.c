// in Zyphr you cant access systic as k_slee() already uses systick
//
//  k_msleep(500);
// The current thread goes to the sleep state.
// Zephyr keeps counting kernel ticks.
// After 500 ms, the scheduler wakes up the thread.
// The LED toggles again.
// 
// SysTick interrupt
//       ↓
// Kernel tick count increases
//       ↓
// k_msleep() checks elapsed time
//       ↓
// Thread wakes up
//       ↓
// Toggle LED