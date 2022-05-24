#include "framework.h"
#include "error.h"
#include <fstream>

void FrameWorkMain(EventPackage *event) {
  Context ctx(event->m_buffer, event);

  auto r = Route::GetInstance();

  r->WarpContext(ctx);

  ctx.Run();
  
  ctx.WriteEvent();
}

void ProcessEvent(EventPackage *event) {
  switch (event->m_eventType) {
  case EVENT_TYPE_READ:
    event->user_data = 0;
    FrameWorkMain(event);
    break;
  default:
    event->m_eventType = EVENT_TYPE_END;
    break;
  }
}

// void onInitWorker() {}

// void onEndWorker() {}