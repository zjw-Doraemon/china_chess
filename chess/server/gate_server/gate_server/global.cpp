#include "global.h"
defer::defer(defer_function end):end(end) {

}
defer::~defer() {
	end();
}