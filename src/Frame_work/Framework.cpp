#include "Framework.h"
#include "EventPackage.h"


void FrameWorkMain(EventPackage *event) {
  Context ctx(event->m_buffer, event);

  auto r = Route::GetInstance();

  r->WrapContext(ctx);

  ctx.Run();
  
  ctx.WriteEvent();
}

void ProcessEvent(EventPackage *event) {
  switch (event->m_eventType) {
  case EVENT_TYPE_READ:
    event->user_data = 0;
    FrameWorkMain(event);
    break;
  case EVENT_TYPE_WRITE:
    break;
  default:
    event->m_eventType = EVENT_TYPE_END;
    break;
  }
}

// void onInitWorker() {}

// void onEndWorker() {}