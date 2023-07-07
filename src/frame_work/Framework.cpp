#include "Framework.h"


void ProcessEvent(EventPackage *event) {
  switch (event->m_eventType) {
    case EVENT_TYPE_READ: {
      event->user_data = 0;
      Context ctx(event->m_buffer, event);
      auto r = Route::GetInstance();
      r->WrapContext(ctx);
      ctx.Run();
      ctx.WriteEvent();
      break;
    }
    default:
      event->m_eventType = EVENT_TYPE_END;
      break;
    }
}
