#include "editor/camera_path.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>

#include "editor/placement.hpp"

namespace loupe::editor {

namespace {

using Quat = std::array<double, 4>; // xyzw

constexpr size_t AXES = 3;
// The fewest keys a path has, as camera_keys.py requires.
constexpr size_t MIN_KEYS = 2;
constexpr float MS_PER_S = 1000.0F;

bool
finite(const Point& point) {
    return std::ranges::all_of(point, [](float value) { return std::isfinite(value); });
}

// Python reckons in double: so does the port of camera_keys.py.
double
wide(float value) {
    return static_cast<double>(value);
}

// camera_keys.py's look rotation: glTF xyzw with local -Z forward and world +Y up,
// in double as Python reckons it. None when look_at is the eye or straight
// above or below it.
std::optional<Quat>
look_rotation(const Point& eye, const Point& look_at) {
    const double fx = wide(look_at[0]) - wide(eye[0]), fy = wide(look_at[1]) - wide(eye[1]);
    const double fz = wide(look_at[2]) - wide(eye[2]);
    if ((fx * fx) + (fy * fy) + (fz * fz) == 0.0 || (fx == 0.0 && fz == 0.0)) {
        return std::nullopt;
    }
    // 0.0 - x, not -x: the keys files write whole numbers, and Python negates an integer 0 to +0, not -0,
    // so a key looking straight down -z turns by +pi as camera_keys.py turns it.
    const double yaw = std::atan2(0.0 - fx, 0.0 - fz);
    const double pitch = std::atan2(fy, std::hypot(fx, fz));
    const double sy = std::sin(yaw / 2), cy = std::cos(yaw / 2);
    const double sp = std::sin(pitch / 2), cp = std::cos(pitch / 2);
    return Quat{cy * sp, sy * cp, -sy * sp, cy * cp};
}

float
distance(const Point& a, const Point& b) {
    return std::hypot(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

} // namespace

bool
CameraPath::unsaved() const {
    return keys != saved;
}

bool
loops(const std::vector<CameraKey>& keys) {
    return keys.size() >= MIN_KEYS && keys.front().eye == keys.back().eye
           && keys.front().look_at == keys.back().look_at;
}

std::optional<std::string>
path_fault(const std::vector<CameraKey>& keys) {
    if (keys.size() < MIN_KEYS) {
        return "a path needs at least two keys";
    }
    for (size_t i = 0; i < keys.size(); i++) {
        const CameraKey& key = keys[i];
        if (!std::isfinite(key.t) || !finite(key.eye) || !finite(key.look_at)) {
            return std::format("key {}: its time and points must be finite numbers", i);
        }
        if (i == 0 ? key.t != 0.0F : key.t <= keys[i - 1].t) {
            return std::format("key {}: times must start at zero and increase strictly", i);
        }
        if (!look_rotation(key.eye, key.look_at)) {
            return std::format("key {}: look_at must differ from eye and not lie straight above or below it", i);
        }
    }
    return std::nullopt;
}

PathCurve
path_curve(const std::vector<CameraKey>& keys) {
    PathCurve curve;
    const size_t n = keys.size();
    const bool loop = loops(keys);
    Quat previous{};
    for (size_t i = 0; i < n; i++) {
        size_t before = i > 0 ? i - 1 : 0, after = std::min(n - 1, i + 1);
        double dt = wide(keys[after].t) - wide(keys[before].t);
        if (loop && (i == 0 || i == n - 1)) {
            before = n - 2;
            after = 1;
            dt = wide(keys[n - 1].t) - wide(keys[before].t) + wide(keys[after].t);
        }
        std::array<float, AXES> tangent{};
        for (size_t axis = 0; axis < AXES; axis++) {
            tangent[axis] = static_cast<float>((wide(keys[after].eye[axis]) - wide(keys[before].eye[axis])) / dt);
        }
        curve.translations.insert(curve.translations.end(), tangent.begin(), tangent.end());
        curve.translations.insert(curve.translations.end(), keys[i].eye.begin(), keys[i].eye.end());
        curve.translations.insert(curve.translations.end(), tangent.begin(), tangent.end());
        Quat q = look_rotation(keys[i].eye, keys[i].look_at).value_or(Quat{0, 0, 0, 1});
        // Each rotation on the side of the one before, so the slerp between them goes the short way.
        if (i > 0 && (q[0] * previous[0]) + (q[1] * previous[1]) + (q[2] * previous[2]) + (q[3] * previous[3]) < 0) {
            std::ranges::transform(q, q.begin(), [](double value) { return -value; });
        }
        previous = q;
        std::ranges::transform(q, std::back_inserter(curve.rotations),
                               [](double value) { return static_cast<float>(value); });
        curve.times.push_back(keys[i].t);
    }
    return curve;
}

float
path_end(const std::vector<CameraKey>& keys) {
    return keys.empty() ? 0.0F : keys.back().t;
}

float
clip_seconds(const std::vector<CameraKey>& keys, uint32_t t_ms) {
    const float end = path_end(keys);
    const float seconds = static_cast<float>(t_ms) / MS_PER_S;
    return end > 0.0F ? std::fmod(seconds, end) : 0.0F;
}

CameraKey
key_on(const PathCurve& curve, const std::vector<CameraKey>& keys, float seconds) {
    const transformf_t pose = pose_on(curve, seconds);
    // The look distance between the keys either side, by time.
    const auto next = std::ranges::upper_bound(keys, seconds, {}, &CameraKey::t);
    const CameraKey& b = next == keys.end() ? keys.back() : *next;
    const CameraKey& a = next == keys.begin() ? keys.front() : *(next - 1);
    const float span = b.t - a.t;
    const float share = span > 0.0F ? std::clamp((seconds - a.t) / span, 0.0F, 1.0F) : 0.0F;
    const float reach =
        distance(a.eye, a.look_at) + ((distance(b.eye, b.look_at) - distance(a.eye, a.look_at)) * share);
    // glTF's camera looks down its local -Z.
    const vec3f_t forward = quatf_rotate(pose.rotation, vec3f_t{0.0F, 0.0F, -1.0F});
    CameraKey key;
    key.t = seconds;
    key.eye = {pose.position.x, pose.position.y, pose.position.z};
    key.look_at = {key.eye[0] + (forward.x * reach), key.eye[1] + (forward.y * reach),
                   key.eye[2] + (forward.z * reach)};
    return key;
}

std::vector<CameraKey>
with_key(const std::vector<CameraKey>& keys, size_t index, const CameraKey& key) {
    std::vector<CameraKey> out = keys;
    const bool loop = loops(keys);
    out[index].t = key.t;
    out[index].eye = key.eye;
    out[index].look_at = key.look_at;
    if (loop && (index == 0 || index == keys.size() - 1)) {
        CameraKey& other = out[index == 0 ? keys.size() - 1 : 0];
        other.eye = key.eye;
        other.look_at = key.look_at;
    }
    return out;
}

float
clamp_time(const std::vector<CameraKey>& keys, size_t index, float t) {
    if (index == 0) {
        return 0.0F;
    }
    const float earliest = keys[index - 1].t + TIME_STEP_S;
    float latest = index + 1 < keys.size() ? keys[index + 1].t - TIME_STEP_S : std::max(t, earliest);
    latest = std::max(latest, earliest);
    return rounded(std::clamp(t, earliest, latest), TIME_STEP_S);
}

std::optional<std::vector<CameraKey>>
with_key_added(const std::vector<CameraKey>& keys, float seconds, size_t* added) {
    if (keys.size() < MIN_KEYS) {
        return std::nullopt;
    }
    seconds = rounded(std::max(seconds, 0.0F), TIME_STEP_S);
    std::vector<CameraKey> out = keys;
    if (seconds >= keys.back().t) {
        // At the end, past it by its last spacing when the time asked is not past it.
        const float spacing = keys.back().t - keys[keys.size() - 2].t;
        CameraKey key = keys.back();
        key.t = seconds > keys.back().t + TIME_STEP_S ? seconds : rounded(keys.back().t + spacing, TIME_STEP_S);
        key.from = -1;
        out.push_back(key);
        *added = out.size() - 1;
        return out;
    }
    const auto next = std::ranges::upper_bound(keys, seconds, {}, &CameraKey::t);
    const auto index = static_cast<size_t>(next - keys.begin());
    if (seconds - keys[index - 1].t < TIME_STEP_S / 2 || keys[index].t - seconds < TIME_STEP_S / 2) {
        return std::nullopt;
    }
    // On the curve, its points rounded as the handles round theirs.
    CameraKey key = key_on(path_curve(keys), keys, seconds);
    for (Point* point : {&key.eye, &key.look_at}) {
        for (float& value : *point) {
            value = rounded(value, UNIT_STEP);
        }
    }
    out.insert(out.begin() + static_cast<std::ptrdiff_t>(index), key);
    *added = index;
    return out;
}

std::optional<std::string>
delete_refusal(const std::vector<CameraKey>& keys, size_t index) {
    if (index == 0) {
        return "the first key is where the path starts, at zero";
    }
    if (keys.size() <= MIN_KEYS) {
        return "a path keeps at least two keys";
    }
    return std::nullopt;
}

} // namespace loupe::editor
