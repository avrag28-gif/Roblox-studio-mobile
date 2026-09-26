import UIKit
import MetalKit
final class RSMViewController: UIViewController {
    private var view3D: MTKView!
    override func loadView() {
        view3D = MTKView(frame: .zero, device: MTLCreateSystemDefaultDevice())
        view3D.clearColor = MTLClearColor(red: 0.055, green: 0.063, blue: 0.08, alpha: 1)
        view3D.enableSetNeedsDisplay = false
        view = view3D
    }
    override func viewDidAppear(_ animated: Bool) { super.viewDidAppear(animated); RSMEngineBridge.start() }
    override func viewWillDisappear(_ animated: Bool) { super.viewWillDisappear(animated); RSMEngineBridge.stop() }
}
enum RSMEngineBridge {
    static func start() { /* shared C++ engine entry point is linked by the iOS target */ }
    static func stop() {}
}