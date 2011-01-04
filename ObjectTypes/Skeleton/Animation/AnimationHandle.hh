/**
 * @brief A handle used to refer to an animation or to a specific frame in an animation
 */
class AnimationHandle
{
public:
  /// Constructs an invalid animation handle (interpreted as handle of the reference pose)
  AnimationHandle() : idAnimation(0), iFrame(0) {};
  /// Constructs a valid handle for the given animation and frame
  AnimationHandle(unsigned int idAnimation, unsigned int iFrame = 0) : idAnimation(idAnimation + 1), iFrame(iFrame) {};
  /// Returns true if the handle is valid
  inline bool isValid() const { return idAnimation > 0; }
  /// Returns the animation index (zero based)
  inline unsigned int animationIndex() const { return idAnimation - 1; }
  
  /// Returns the selected frame (zero based)
  inline unsigned int frame() const { return iFrame; }
  /// Sets the current animation frame (not failsafe)
  inline void setFrame(unsigned int _iFrame) { iFrame = _iFrame; }

  /// Returns to the first frame
  inline void firstFrame() { iFrame = 0; }
  /// Increases the handle to the next frame (not failsafe)
  inline void operator++() { ++iFrame; }
  /// Decreases the handle to the previous frame (not failsafe)
  inline void operator--() { --iFrame; }

  inline bool operator==(const AnimationHandle &rhs) { return (idAnimation == rhs.idAnimation) && (iFrame == rhs.iFrame); }
  inline bool operator!=(const AnimationHandle &rhs) { return (idAnimation != rhs.idAnimation) || (iFrame != rhs.iFrame); }

private:
  /// The one based index of the animation, set to 0 for invalid (or reference pose)
  unsigned int idAnimation;
  /// The frame number
  unsigned int iFrame;
};
